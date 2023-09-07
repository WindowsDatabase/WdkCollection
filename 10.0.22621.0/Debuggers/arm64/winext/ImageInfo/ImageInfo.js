"use strict";

/*************************************************

ImageInfo.js

Debugger extension which parses PE images mapped into the address
space of the target and presents the resources within those images
on the debugger's module object.

*************************************************/

//
// Permanently remove the generic object "[Object object]" string conversion.
//
delete Object.prototype.toString;

//*************************************************
// Type System Utilities:
//

//
// Unfortunately, the PE structures are not in the public symbols for ntdll despite being
// publicly documented on MSDN.  Include a 'type creator' utility.  At some point in the future
// when JsProvider supports modules, this should be extracted into such.
//
// This also helps in circumstances where the debugger is opened "-z" against a PE image.  Even
// with private symbols, the ntdll symbols (which contain all the PE definitions) would not be
// loaded and this would fail.
//

var __keyModule = "nt";

var __syntheticTypes = {};
var __globalScope = this;

var __forceUsePublicSymbols = false;
var __usePrivateSymbols = !__forceUsePublicSymbols;
var __checkedPrivateSymbols = false;

// __getSyntheticTypeSize:
//
// Gets the size of a synthetic type.
//
function __getSyntheticTypeSize(typeName, contextInheritorModule)
{
    var typeInfo = __syntheticTypes[typeName];
    var size = typeInfo.size;
    if (size === undefined)
    {
        size = 0;
        var curBit = 0;
        var addBit = true;
        for (var field of typeInfo.descriptor)
        {
            var fieldSize = __getSyntheticFieldSize(field, contextInheritorModule);
            var isBitField = !(field.bitLength === undefined);
            if (isBitField)
            {
                var originalFieldSize = fieldSize;
                if (!addBit)
                {
                    //
                    // Do not count subsequent portions of the bit field in the overall type size.
                    //
                    fieldSize = 0;
                }
                curBit += field.bitLength;
                if (curBit >= originalFieldSize * 8)
                {
                    curBit = 0;
                    addBit = true;
                }
                else
                {
                    addBit = false;
                }
            }
            else
            {
                curBit = 0;
                addBit = true;
            }

            if (typeInfo.isUnion)
            {
                if (fieldSize > size)
                {
                    size = fieldSize;
                }
            }
            else
            {
                size += fieldSize;
            }
        }
        typeInfo.size = size;
    }
    return size;
}

// __getSyntheticFieldSize:
//
// Returns the size of a field (whether synthetic or native) of a synthetic type.
//
function __getSyntheticFieldSize(field, contextInheritorModule)
{
    if (field.fieldType === undefined)
    {
        return __getSyntheticTypeSize(field.fieldSyntheticType, contextInheritorModule);
    }
    else
    {
        return host.evaluateExpression("sizeof(" + field.fieldType + ")", contextInheritorModule);
    }
}

// __getFieldValue:
//
// Returns the value of a field (whether synthetic or native) of a synthetic type.
//
function __getSyntheticFieldValue(field, addr, contextInheritorModule)
{
    if (field.fieldType === undefined)
    {
        var typeClass = __syntheticTypes[field.fieldSyntheticType].classObject;

        var arraySize = field.syntheticArraySize;
        if (arraySize === undefined)
        {
            return new typeClass(addr, contextInheritorModule);
        }
        else
        {
            var typeSize = __getSyntheticTypeSize(field.fieldSyntheticType, contextInheritorModule);
            var result = [];
            for (var i = 0; i < arraySize; ++i)
            {
                var entry = new typeClass(addr, contextInheritorModule);
                result.push(entry);
                addr = addr.add(typeSize);
            }
            return result;
        }
    }
    else
    {
        //
        // fieldType references basic types that should be present in **ANY** symbolic information.
        // Just grab the first module as the "reference module" for this purpose.  We cannot grab
        // "ntdll" generically as we want to avoid a situation in which the debugger opens a module (-z ...)
        // from failing.
        //
        var moduleName = contextInheritorModule.__ComparisonName;
        var typeObject = host.getModuleType(moduleName, field.fieldType, contextInheritorModule);
        var result = host.createTypedObject(addr, typeObject);

        //
        // If this is a synthetic bit field, do the appropriate mask and shift.
        //
        if (field.bitLength)
        {
            var size = host.evaluateExpression("sizeof(" + field.fieldType + ")");
            var one = 1;
            var allBits = 0xFFFFFFFF;
            if (size > 4)
            {
                one = host.Int64(1);
                allBits = host.Int64(0xFFFFFFFF, 0xFFFFFFFF);
            }

            var topBit = field.startingBit + field.bitLength;
            var topMask = one.bitwiseShiftLeft(topBit).subtract(1);

            result = result.bitwiseAnd(topMask);
            if (field.startingBit > 0)
            {
                var bottomMask = one.bitwiseShiftLeft(field.startingBit).subtract(1).bitwiseXor(allBits);
                result = result.bitwiseAnd(bottomMask);
                result = result.bitwiseShiftRight(field.startingBit);
            }
        }

        return result;
    }
}

// __preprocessTypeDescriptor:
//
// Preprocess the type descriptor and make certain things easier.
//
function __preprocessTypeDescriptor(typeDescriptor)
{
    for (var field of typeDescriptor)
    {
        var synType = field.fieldSyntheticType;
        if (!(synType === undefined))
        {
            //
            // If the synthetic type is an "array", we need to get the base type and actually tag it as an array
            //
            synType = synType.trim();
            if (synType.endsWith("]"))
            {
                //
                // Extract the array portion.
                //
                var extractor = /(.*)\[(\d+)\]$/;
                var results = extractor.exec(synType);
                field.fieldSyntheticType = results[1].trim();
                field.syntheticArraySize = parseInt(results[2]);
            }
        }
    }
}

// __embedType:
//
// Takes a field (for what might be an unnamed struct/union) and embeds it in the outer structure.
//
function __embedType(outerObject, fieldValue, contextInheritorModule)
{
    var names = Object.getOwnPropertyNames(fieldValue);
    for (var name of names)
    {
        //
        // Do *NOT* overwrite the projected things we place on the objects.
        //
        if (name != "targetLocation" && name != "targetSize")
        {
            outerObject[name] = fieldValue[name];
        }
    }
}

// __defineSyntheticType:
//
// Defines a "synthetic" type based on a descriptor (list of fields).  For a structure, FOO, this would
// allow syntax like this:
//
//     var myFoo = new __FOO(location);
//
// and then use of any synthetic field much as if it were an actual native object created with createTypedObject.
//
function __defineSyntheticType(typeName, typeDescriptor)
{
    __preprocessTypeDescriptor(typeDescriptor);

    class typeClass
    {
        constructor(addr, contextInheritorModule)
        {
            var curBit = 0;
            var fieldSize = 0;
            var addr64 = host.Int64(addr);
            this.targetLocation = addr64;
            this.targetSize = __getSyntheticTypeSize(typeName, contextInheritorModule);
            for (var field of typeDescriptor)
            {
                var fldSize = __getSyntheticFieldSize(field, contextInheritorModule);
                var isBitField = !(field.bitLength === undefined);
                if (isBitField)
                {
                    if (fieldSize == 0)
                    {
                        curBit = 0;
                        fieldSize = fldSize;
                    }
                    field.startingBit = curBit;
                }

                var fldValue = __getSyntheticFieldValue(field, addr64, contextInheritorModule);

                var fldName = field.fieldName;
                if (fldName === undefined)
                {
                    __embedType(this, fldValue, contextInheritorModule);
                }
                else
                {
                    this[field.fieldName] = fldValue;
                }

                if (isBitField)
                {
                    curBit += field.bitLength;
                    if (curBit >= fieldSize * 8)
                    {
                        curBit = 0;
                        fieldSize = 0;
                        addr64 = addr64.add(fldSize);
                    }
                }
                else
                {
                    addr64 = addr64.add(fldSize);
                }
            }
        }
    };

    __syntheticTypes[typeName] =
    {
        classObject: typeClass,
        descriptor: typeDescriptor,
        isUnion: false
    }

    __globalScope["__" + typeName] = typeClass;

    return typeClass;
}

// __defineSyntheticUnion:
//
// Defines a "synthetic" union based on a descriptor (list of all fields in the union).  For a union, FOO, this would
// allow syntax like this:
//
//     var myUnionFoo = new __FOO(location);
//
// and then use of any synthetic field much as if it were an actual native object created with createTypedObject.
//
function __defineSyntheticUnion(typeName, typeDescriptor)
{
    __preprocessTypeDescriptor(typeDescriptor);

    class typeClass
    {
        constructor(addr, contextInheritorModule)
        {
            var largestSize = 0;
            var curBit = 0;
            var fieldSize = 0;
            var addr64 = host.Int64(addr);
            this.targetLocation = addr64;
            this.targetSize = __getSyntheticTypeSize(typeName, contextInheritorModule);
            for (var field of typeDescriptor)
            {
                var fldSize = __getSyntheticFieldSize(field, contextInheritorModule);
                var isBitField = !(field.bitLength === undefined);
                if (isBitField)
                {
                    if (fieldSize == 0)
                    {
                        curBit = 0;
                        fieldSize = fldSize;
                    }
                    field.startingBit = curBit;
                }

                var fldValue = __getSyntheticFieldValue(field, addr64, contextInheritorModule);

                var fldName = field.fieldName;
                if (fldName === undefined)
                {
                    __embedType(this, fldValue, contextInheritorModule);
                }
                else
                {
                    this[field.fieldName] = fldValue;
                }

                if (isBitField)
                {
                    curBit += field.bitLength;
                    if (curBit >= fieldSize * 8)
                    {
                        curBit = 0;
                        fieldSize = 0;
                    }
                }
            }
        }
    };

    __syntheticTypes[typeName] =
    {
        classObject: typeClass,
        descriptor: typeDescriptor,
        isUnion: true
    }

    __globalScope["__" + typeName] = typeClass;
}

//
// Define the core PE data structures that we must have "type information" for in order for the rest of the
// script to work properly.
//
// Documentation on these can be found on MSDN: https://msdn.microsoft.com/en-us/library/ms809762.aspx
// This is "Peering Inside the PE: A Tour of the Win32 Portable Executable File Format"
//
__defineSyntheticType("_IMAGE_DOS_HEADER",            [{fieldName: "e_magic", fieldType: "unsigned short"},
                                                       {fieldName: "e_cblp", fieldType: "unsigned short"},
                                                       {fieldName: "e_cp", fieldType: "unsigned short"},
                                                       {fieldName: "e_crlc", fieldType: "unsigned short"},
                                                       {fieldName: "e_cparhdr", fieldType: "unsigned short"},
                                                       {fieldName: "e_minalloc", fieldType: "unsigned short"},
                                                       {fieldName: "e_maxalloc", fieldType: "unsigned short"},
                                                       {fieldName: "e_ss", fieldType: "unsigned short"},
                                                       {fieldName: "e_sp", fieldType: "unsigned short"},
                                                       {fieldName: "e_csum", fieldType: "unsigned short"},
                                                       {fieldName: "e_ip", fieldType: "unsigned short"},
                                                       {fieldName: "e_cs", fieldType: "unsigned short"},
                                                       {fieldName: "e_lfarlc", fieldType: "unsigned short"},
                                                       {fieldName: "e_ovno", fieldType: "unsigned short"},
                                                       {fieldName: "e_res", fieldType: "unsigned short[4]"},
                                                       {fieldName: "e_oemid", fieldType: "unsigned short"},
                                                       {fieldName: "e_oeminfo", fieldType: "unsigned short"},
                                                       {fieldName: "e_res2", fieldType: "unsigned short[10]"},
                                                       {fieldName: "e_lfanew", fieldType: "unsigned long"}]);

__defineSyntheticType("_IMAGE_FILE_HEADER",           [{fieldName: "Machine", fieldType: "unsigned short"},
                                                       {fieldName: "NumberOfSections", fieldType: "unsigned short"},
                                                       {fieldName: "TimeDateStamp", fieldType: "unsigned long"},
                                                       {fieldName: "PointerToSymbolTable", fieldType: "unsigned long"},
                                                       {fieldName: "NumberOfSymbols", fieldType: "unsigned long"},
                                                       {fieldName: "SizeOfOptionalHeader", fieldType: "unsigned short"},
                                                       {fieldName: "Characteristics", fieldType: "unsigned short"}]);

__defineSyntheticType("_IMAGE_OPTIONAL_HEADER32",     [{fieldName: "Magic", fieldType: "unsigned short"},
                                                       {fieldName: "MajorLinkerVersion", fieldType: "unsigned char"},
                                                       {fieldName: "MinorLinkerVersion", fieldType: "unsigned char"},
                                                       {fieldName: "SizeOfCode", fieldType: "unsigned long"},
                                                       {fieldName: "SizeOfInitializedData", fieldType: "unsigned long"},
                                                       {fieldName: "SizeOfUninitializedData", fieldType: "unsigned long"},
                                                       {fieldName: "AddressOfEntryPoint", fieldType: "unsigned long"},
                                                       {fieldName: "BaseOfCode", fieldType: "unsigned long"},
                                                       {fieldName: "BaseOfData", fieldType: "unsigned long"},
                                                       {fieldName: "ImageBase", fieldType: "unsigned long"},
                                                       {fieldName: "SectionAlignment", fieldType: "unsigned long"},
                                                       {fieldName: "FileAlignment", fieldType: "unsigned long"},
                                                       {fieldName: "MajorOperatingSystemVersion", fieldType: "unsigned short"},
                                                       {fieldName: "MinorOperatingSystemVersion", fieldType: "unsigned short"},
                                                       {fieldName: "MajorImageVersion", fieldType: "unsigned short"},
                                                       {fieldName: "MinorImageVersion", fieldType: "unsigned short"},
                                                       {fieldName: "MajorSubsystemVersion", fieldType: "unsigned short"},
                                                       {fieldName: "MinorSubsystemVersion", fieldType: "unsigned short"},
                                                       {fieldName: "Win32VersionValue", fieldType: "unsigned long"},
                                                       {fieldName: "SizeOfImage", fieldType: "unsigned long"},
                                                       {fieldName: "SizeOfHeaders", fieldType: "unsigned long"},
                                                       {fieldName: "CheckSum", fieldType: "unsigned long"},
                                                       {fieldName: "Subsystem", fieldType: "unsigned short"},
                                                       {fieldName: "DllCharacteristics", fieldType: "unsigned short"},
                                                       {fieldName: "SizeOfStackReserve", fieldType: "unsigned long"},
                                                       {fieldName: "SizeOfStackCommit", fieldType: "unsigned long"},
                                                       {fieldName: "SizeOfHeapReserve", fieldType: "unsigned long"},
                                                       {fieldName: "SizeOfHeapCommit", fieldType: "unsigned long"},
                                                       {fieldName: "LoaderFlags", fieldType: "unsigned long"},
                                                       {fieldName: "NumberOfRvaAndSizes", fieldType: "unsigned long"},
                                                       {fieldName: "DataDirectory", fieldSyntheticType: "_IMAGE_DATA_DIRECTORY[16]"}]);

__defineSyntheticType("_IMAGE_OPTIONAL_HEADER64",     [{fieldName: "Magic", fieldType: "unsigned short"},
                                                       {fieldName: "MajorLinkerVersion", fieldType: "unsigned char"},
                                                       {fieldName: "MinorLinkerVersion", fieldType: "unsigned char"},
                                                       {fieldName: "SizeOfCode", fieldType: "unsigned long"},
                                                       {fieldName: "SizeOfInitializedData", fieldType: "unsigned long"},
                                                       {fieldName: "SizeOfUninitializedData", fieldType: "unsigned long"},
                                                       {fieldName: "AddressOfEntryPoint", fieldType: "unsigned long"},
                                                       {fieldName: "BaseOfCode", fieldType: "unsigned long"},
                                                       {fieldName: "ImageBase", fieldType: "unsigned __int64"},
                                                       {fieldName: "SectionAlignment", fieldType: "unsigned long"},
                                                       {fieldName: "FileAlignment", fieldType: "unsigned long"},
                                                       {fieldName: "MajorOperatingSystemVersion", fieldType: "unsigned short"},
                                                       {fieldName: "MinorOperatingSystemVersion", fieldType: "unsigned short"},
                                                       {fieldName: "MajorImageVersion", fieldType: "unsigned short"},
                                                       {fieldName: "MinorImageVersion", fieldType: "unsigned short"},
                                                       {fieldName: "MajorSubsystemVersion", fieldType: "unsigned short"},
                                                       {fieldName: "MinorSubsystemVersion", fieldType: "unsigned short"},
                                                       {fieldName: "Win32VersionValue", fieldType: "unsigned long"},
                                                       {fieldName: "SizeOfImage", fieldType: "unsigned long"},
                                                       {fieldName: "SizeOfHeaders", fieldType: "unsigned long"},
                                                       {fieldName: "CheckSum", fieldType: "unsigned long"},
                                                       {fieldName: "Subsystem", fieldType: "unsigned short"},
                                                       {fieldName: "DllCharacteristics", fieldType: "unsigned short"},
                                                       {fieldName: "SizeOfStackReserve", fieldType: "unsigned __int64"},
                                                       {fieldName: "SizeOfStackCommit", fieldType: "unsigned __int64"},
                                                       {fieldName: "SizeOfHeapReserve", fieldType: "unsigned __int64"},
                                                       {fieldName: "SizeOfHeapCommit", fieldType: "unsigned __int64"},
                                                       {fieldName: "LoaderFlags", fieldType: "unsigned long"},
                                                       {fieldName: "NumberOfRvaAndSizes", fieldType: "unsigned long"},
                                                       {fieldName: "DataDirectory", fieldSyntheticType: "_IMAGE_DATA_DIRECTORY[16]"}]);

__defineSyntheticType("_IMAGE_DATA_DIRECTORY",        [{fieldName: "VirtualAddress", fieldType: "unsigned long"},
                                                       {fieldName: "Size", fieldType: "unsigned long"}]);

__defineSyntheticType("_IMAGE_RESOURCE_DIRECTORY",    [{fieldName: "Characteristics", fieldType: "unsigned long"},
                                                       {fieldName: "TimeDateStamp", fieldType: "unsigned long"},
                                                       {fieldName: "MajorVersion", fieldType: "unsigned short"},
                                                       {fieldName: "MinorVersion", fieldType: "unsigned short"},
                                                       {fieldName: "NumberOfNamedEntries", fieldType: "unsigned short"},
                                                       {fieldName: "NumberOfIdEntries", fieldType: "unsigned short"}]);

__defineSyntheticType("_IMAGE_EXPORT_DIRECTORY",      [{fieldName: "Characteristics", fieldType: "unsigned long"},
                                                       {fieldName: "TimeDateStamp", fieldType: "unsigned long"},
                                                       {fieldName: "MajorVersion", fieldType: "unsigned short"},
                                                       {fieldName: "MinorVersion", fieldType: "unsigned short"},
                                                       {fieldName: "Name", fieldType: "unsigned long"},
                                                       {fieldName: "Base", fieldType: "unsigned long"},
                                                       {fieldName: "NumberOfFunctions", fieldType: "unsigned long"},
                                                       {fieldName: "NumberOfNames", fieldType: "unsigned long"},
                                                       {fieldName: "AddressOfFunctions", fieldType: "unsigned long"},
                                                       {fieldName: "AddressOfNames", fieldType: "unsigned long"},
                                                       {fieldName: "AddressOfNameOrdinals", fieldType: "unsigned long"}]);

__defineSyntheticUnion("__UNNAMED_IIDUNION",          [{fieldName: "OriginalFirstThunk", fieldType: "unsigned long"},
                                                       {fieldName: "Characteristics", fieldType: "unsigned long"}]);

__defineSyntheticType("_IMAGE_IMPORT_DESCRIPTOR",     [{fieldSyntheticType: "__UNNAMED_IIDUNION"},
                                                       {fieldName: "TimeDateStamp", fieldType: "unsigned long"},
                                                       {fieldName: "ForwarderChain", fieldType: "unsigned long"},
                                                       {fieldName: "Name", fieldType: "unsigned long"},
                                                       {fieldName: "FirstThunk", fieldType: "unsigned long"}]);

__defineSyntheticType("_IMAGE_RESOURCE_DIR_STRING_U", [{fieldName: "Length", fieldType: "unsigned short"},
                                                       {fieldName: "NameString", fieldType: "wchar_t[1]"}]);

__defineSyntheticType("_IMAGE_RESOURCE_DATA_ENTRY",   [{fieldName: "OffsetToData", fieldType: "unsigned long"},
                                                       {fieldName: "Size", fieldType: "unsigned long"},
                                                       {fieldName: "CodePage", fieldType: "unsigned long"},
                                                       {fieldName: "Reserved", fieldType: "unsigned long"}]);

__defineSyntheticType("__UNNAMED_IDD_ATTSTRUCT",      [{fieldName: "RvaBased", fieldType: "unsigned long", bitLength: 1},
                                                       {fieldName: "ReservedAttributes", fieldType: "unsigned long", bitLength: 31}]);

__defineSyntheticUnion("__IDD_ATTUNION",              [{fieldName: "AllAttributes", fieldType: "unsigned long"},
                                                       {fieldSyntheticType: "__UNNAMED_IDD_ATTSTRUCT"}]);

__defineSyntheticType("_IMAGE_DELAYLOAD_DESCRIPTOR",  [{fieldName: "Attributes", fieldSyntheticType: "__IDD_ATTUNION"},
                                                       {fieldName: "DllNameRVA", fieldType: "unsigned long"},
                                                       {fieldName: "ModuleHandleRVA", fieldType: "unsigned long"},
                                                       {fieldName: "ImportAddressTableRVA", fieldType: "unsigned long"},
                                                       {fieldName: "ImportNameTableRVA", fieldType: "unsigned long"},
                                                       {fieldName: "BoundImportAddressTableRVA", fieldType: "unsigned long"},
                                                       {fieldName: "UnloadInformationTableRVA", fieldType: "unsigned long"},
                                                       {fieldName: "TimeDateStamp", fieldType: "unsigned long"}]);

__defineSyntheticType("__UNNAMED_IRDE_UNION1_STRUCT", [{fieldName: "NameOffset", fieldType: "unsigned long", bitLength: 31},
                                                       {fieldName: "NameIsString", fieldType: "unsigned long", bitLength: 1}]);


__defineSyntheticUnion("__UNNAMED_IRDE_UNION1",       [{fieldSyntheticType: "__UNNAMED_IRDE_UNION1_STRUCT"},
                                                       {fieldName: "Name", fieldType: "unsigned long"},
                                                       {fieldName: "Id", fieldType: "unsigned short"}]);

__defineSyntheticType("__UNNAMED_IRDE_UNION2_STRUCT", [{fieldName: "OffsetToDirectory", fieldType: "unsigned long", bitLength: 31},
                                                       {fieldName: "DataIsDirectory", fieldType: "unsigned long", bitLength: 1}]);

__defineSyntheticUnion("__UNNAMED_IRDE_UNION2",       [{fieldName: "OffsetToData", fieldType: "unsigned long"},
                                                       {fieldSyntheticType: "__UNNAMED_IRDE_UNION2_STRUCT"}]);


__defineSyntheticType("_IMAGE_RESOURCE_DIRECTORY_ENTRY", [{fieldSyntheticType: "__UNNAMED_IRDE_UNION1"},
                                                          {fieldSyntheticType: "__UNNAMED_IRDE_UNION2"}]);

__defineSyntheticType("VS_FIXEDFILEINFO", [{fieldName: "dwSignature", fieldType: "unsigned long"},
                                           {fieldName: "dwStrucVersion", fieldType: "unsigned long"},
                                           {fieldName: "dwFileVersionMS", fieldType: "unsigned long"},
                                           {fieldName: "dwFileVersionLS", fieldType: "unsigned long"},
                                           {fieldName: "dwProductVersionMS", fieldType: "unsigned long"},
                                           {fieldName: "dwProductVersionLS", fieldType: "unsigned long"},
                                           {fieldName: "dwFileFlagsMask", fieldType: "unsigned long"},
                                           {fieldName: "dwFileFlags", fieldType: "unsigned long"},
                                           {fieldName: "dwFileOS", fieldType: "unsigned long"},
                                           {fieldName: "dwFileType", fieldType: "unsigned long"},
                                           {fieldName: "dwFileSubtype", fieldType: "unsigned long"},
                                           {fieldName: "dwFileDateMS", fieldType: "unsigned long"},
                                           {fieldName: "dwFileDateLS", fieldType: "unsigned long"}]);

__defineSyntheticType("_IMAGE_DEBUG_DIRECTORY", [{fieldName: "Characteristics", fieldType: "unsigned long"},
                                                 {fieldName: "TimeDateStamp", fieldType: "unsigned long"},
                                                 {fieldName: "MajorVersion", fieldType: "unsigned short"},
                                                 {fieldName: "MinorVersion", fieldType: "unsigned short"},
                                                 {fieldName: "Type", fieldType: "unsigned long"},
                                                 {fieldName: "SizeOfData", fieldType: "unsigned long"},
                                                 {fieldName: "AddressOfRawData", fieldType: "unsigned long"},
                                                 {fieldName: "PointerToRawData", fieldType: "unsigned long"}]);

__defineSyntheticType("___ELF_HEADER32", [{fieldName: "Identifier", fieldType: "unsigned char[4]"}, // e_ident 0-3
                                          {fieldName: "Class", fieldType: "unsigned char"},         // .. EI_CLASS
                                          {fieldName: "Endianness", fieldType: "unsigned char"},    // .. EI_DATA
                                          {fieldName: "Version", fieldType: "unsigned char"},       // .. EI_VERSION
                                          {fieldName: "OSABI", fieldType: "unsigned char"},         // .. EI_OSABI
                                          {fieldName: "ABIVersion", fieldType: "unsigned char"},    // .. EI_ABIVERSION
                                          {fieldName: "Pad", fieldType: "unsigned char[7]"},        // .. EI_PAD
                                          {fieldName: "Type", fieldType: "unsigned short"},         // e_type
                                          {fieldName: "Machine", fieldType: "unsigned short"},      // e_machine
                                          {fieldName: "ELFVersion", fieldType: "unsigned long"},    // e_version
                                          {fieldName: "Entry", fieldType: "unsigned long"},         // e_entry
                                          {fieldName: "OffsetPHdr", fieldType: "unsigned long"},    // e_phoff
                                          {fieldName: "OffsetSHdr", fieldType: "unsigned long"},    // e_shoff
                                          {fieldName: "Flags", fieldType: "unsigned long"},         // e_flags
                                          {fieldName: "EHSize", fieldType: "unsigned short"},       // e_ehsize
                                          {fieldName: "PHEntSize", fieldType: "unsigned short"},    // e_phentsize
                                          {fieldName: "PHNum", fieldType: "unsigned short"},        // e_phnum
                                          {fieldName: "SHEntSize", fieldType: "unsigned short"},    // e_shentsize
                                          {fieldName: "SHNum", fieldType: "unsigned short"},        // e_shnum
                                          {fieldName: "SHStrNdx", fieldType: "unsigned short"}]);   // e_shstrndx

__defineSyntheticType("___ELF_HEADER64", [{fieldName: "Identifier", fieldType: "unsigned char[4]"}, // e_ident 0-3
                                          {fieldName: "Class", fieldType: "unsigned char"},         // .. EI_CLASS
                                          {fieldName: "Endianness", fieldType: "unsigned char"},    // .. EI_DATA
                                          {fieldName: "Version", fieldType: "unsigned char"},       // .. EI_VERSION
                                          {fieldName: "OSABI", fieldType: "unsigned char"},         // .. EI_OSABI
                                          {fieldName: "ABIVersion", fieldType: "unsigned char"},    // .. EI_ABIVERSION
                                          {fieldName: "Pad", fieldType: "unsigned char[7]"},        // .. EI_PAD
                                          {fieldName: "Type", fieldType: "unsigned short"},         // e_type
                                          {fieldName: "Machine", fieldType: "unsigned short"},      // e_machine
                                          {fieldName: "ELFVersion", fieldType: "unsigned long"},    // e_version
                                          {fieldName: "Entry", fieldType: "unsigned __int64"},      // e_entry
                                          {fieldName: "OffsetPHdr", fieldType: "unsigned __int64"}, // e_phoff
                                          {fieldName: "OffsetSHdr", fieldType: "unsigned __int64"}, // e_shoff
                                          {fieldName: "Flags", fieldType: "unsigned long"},         // e_flags
                                          {fieldName: "EHSize", fieldType: "unsigned short"},       // e_ehsize
                                          {fieldName: "PHEntSize", fieldType: "unsigned short"},    // e_phentsize
                                          {fieldName: "PHNum", fieldType: "unsigned short"},        // e_phnum
                                          {fieldName: "SHEntSize", fieldType: "unsigned short"},    // e_shentsize
                                          {fieldName: "SHNum", fieldType: "unsigned short"},        // e_shnum
                                          {fieldName: "SHStrNdx", fieldType: "unsigned short"}]);   // e_shstrndx

__defineSyntheticType("___ELF_SECTIONHEADER32", [{fieldName: "Name", fieldType: "unsigned long"},    // sh_name
                                                 {fieldName: "Type", fieldType: "unsigned long"},    // sh_type
                                                 {fieldName: "Flags", fieldType: "unsigned long"},   // sh_flags
                                                 {fieldName: "Address", fieldType: "unsigned long"}, // sh_addr
                                                 {fieldName: "Offset", fieldType: "unsigned long"},  // sh_offset
                                                 {fieldName: "Size", fieldType: "unsigned long"},    // sh_size
                                                 {fieldName: "Link", fieldType: "unsigned long"},    // sh_link
                                                 {fieldName: "Info", fieldType: "unsigned long"},    // sh_info
                                                 {fieldName: "Align", fieldType: "unsigned long"},   // sh_addralign
                                                 {fieldName: "EntSize", fieldType: "unsigned long"}]); // sh_entsize
 
__defineSyntheticType("___ELF_SECTIONHEADER64", [{fieldName: "Name", fieldType: "unsigned long"},    // sh_name
                                                 {fieldName: "Type", fieldType: "unsigned long"},    // sh_type
                                                 {fieldName: "Flags", fieldType: "unsigned __int64"}, // sh_flags
                                                 {fieldName: "Address", fieldType: "unsigned __int64"}, // sh_addr
                                                 {fieldName: "Offset", fieldType: "unsigned __int64"}, // sh_offset
                                                 {fieldName: "Size", fieldType: "unsigned __int64"}, // sh_size
                                                 {fieldName: "Link", fieldType: "unsigned long"},    // sh_link
                                                 {fieldName: "Info", fieldType: "unsigned long"},    // sh_info
                                                 {fieldName: "Align", fieldType: "unsigned __int64"}, // sh_addralign
                                                 {fieldName: "EntSize", fieldType: "unsigned __int64"}]); // sh_entsize

__defineSyntheticType("___ELF_PROGRAMHEADER32", [{fieldName: "Type", fieldType: "unsigned long"},     // p_type
                                                 {fieldName: "Offset", fieldType: "unsigned long"},   // p_offset
                                                 {fieldName: "VAddr", fieldType: "unsigned long"},    // p_vaddr
                                                 {fieldName: "PAddr", fieldType: "unsigned long"},    // p_paddr
                                                 {fieldName: "FileSize", fieldType: "unsigned long"}, // p_filesz
                                                 {fieldName: "MemSize", fieldType: "unsigned long"},  // p_memsz
                                                 {fieldName: "Flags", fieldType: "unsigned long"},    // p_flags
                                                 {fieldName: "Align", fieldType: "unsigned long"}]);  // p_align

__defineSyntheticType("___ELF_PROGRAMHEADER64", [{fieldName: "Type", fieldType: "unsigned long"},     // p_type
                                                 {fieldName: "Flags", fieldType: "unsigned long"},    // p_flags
                                                 {fieldName: "Offset", fieldType: "unsigned __int64"},// p_offset
                                                 {fieldName: "VAddr", fieldType: "unsigned __int64"}, // p_vaddr
                                                 {fieldName: "PAddr", fieldType: "unsigned __int64"}, // p_paddr
                                                 {fieldName: "FileSize", fieldType: "unsigned __int64"},// p_filesz
                                                 {fieldName: "MemSize", fieldType: "unsigned __int64"},// p_memsz
                                                 {fieldName: "Align", fieldType: "unsigned __int64"}]);// p_align

__defineSyntheticType("___ELF_NOTE", [{fieldName: "NameSize", fieldType: "unsigned long"},            // n_namesz
                                      {fieldName: "DescSize", fieldType: "unsigned long"},            // n_descsz
                                      {fieldName: "Type", fieldType: "unsigned long"}]);              // n_type

__defineSyntheticType("___ELF_DYNAMICENTRY32", [{fieldName: "Tag", fieldType: "unsigned long"},
                                                {fieldName: "Val", fieldType: "unsigned long"}]);

__defineSyntheticType("___ELF_DYNAMICENTRY64", [{fieldName: "Tag", fieldType: "unsigned __int64"},
                                                {fieldName: "Val", fieldType: "unsigned __int64"}]);

__defineSyntheticType("___ELF_RDEBUG64", [{fieldName: "Version", fieldType: "int"},
                                          {fieldName: "Padding", fieldType: "int"},
                                          {fieldName: "AddrRMap", fieldType: "unsigned __int64"},
                                          {fieldName: "RBrk", fieldType: "unsigned __int64"}]);

__defineSyntheticType("___ELF_RDEBUG32", [{fieldName: "Version", fieldType: "int"},
                                          {fieldName: "Padding", fieldType: "int"},
                                          {fieldName: "AddrRMap", fieldType: "unsigned long"},
                                          {fieldName: "RBrk", fieldType: "unsigned long"}]);

__defineSyntheticType("___ELF_LINKMAPENTRY64", [{fieldName: "LAddr", fieldType: "unsigned __int64"},
                                                {fieldName: "LName", fieldType: "unsigned __int64"},
                                                {fieldName: "LDyn", fieldType: "unsigned __int64"},
                                                {fieldName: "LNext", fieldType: "unsigned __int64"}]);

__defineSyntheticType("___ELF_LINKMAPENTRY32", [{fieldName: "LAddr", fieldType: "unsigned long"},
                                                {fieldName: "LName", fieldType: "unsigned long"},
                                                {fieldName: "LDyn", fieldType: "unsigned long"},
                                                {fieldName: "LNext", fieldType: "unsigned long"}]);
 
var __guidType = __defineSyntheticType("GUID", [{fieldName: "Data1", fieldType: "unsigned long"},
                                                {fieldName: "Data2", fieldType: "unsigned short"},
                                                {fieldName: "Data3", fieldType: "unsigned short"},
                                                {fieldName: "Data4", fieldType: "unsigned char[8]"}]);

// __convertHex:
//
// Does a hex string conversion forcing the size to be the number of bits specified.
//
function __convertHex(val, bits)
{
    var strVal = "";
    bits -= 4;
    while(true)
    {
        var nyb = (val >> bits) & 0xF;
        if (nyb >= 0 && nyb <= 9)
        {
            strVal += String.fromCharCode(48 + nyb);
        }
        else
        {
            strVal += String.fromCharCode(55 + nyb);
        }
        if (bits < 4)
        {
            break;
        }
        bits -= 4;
    }
    return strVal;
}

//
// If we do not have the key module, we need a string conversion function for GUIDs.
//
__guidType.prototype.toString = function()
{
    var guidStr = "{";
    guidStr =  "{" +
               __convertHex(this.Data1, 32) + "-" + 
               __convertHex(this.Data2, 16) + "-" +
               __convertHex(this.Data3, 16) + "-" +
               __convertHex(this.Data4[0], 8) + __convertHex(this.Data4[1], 8) + "-" +
               __convertHex(this.Data4[2], 8) +
               __convertHex(this.Data4[3], 8) +
               __convertHex(this.Data4[4], 8) +
               __convertHex(this.Data4[5], 8) +
               __convertHex(this.Data4[6], 8) +
               __convertHex(this.Data4[7], 8) +
               "}";

    return guidStr;
};

// __createSyntheticTypedObject:
//
// Wrapper that forces creation of a type based on our synthetic type tables.
//
function __createSyntheticTypedObject(addrLoc, moduleName, typeName, contextInheritorModule, syntheticAltName)
{
    var creatorName = typeName;
    if (!(syntheticAltName === undefined))
    {
        creatorName = syntheticAltName;
    }
    var typeClass = __globalScope["__" + creatorName];
    var result = new typeClass(addrLoc, contextInheritorModule);
    return result;
}

// __createTypedObject:
//
// Wrapper for host.createTypedObject which will either do so or will use our synthetic symbols if a public
// PDB is loaded.
//
function __createTypedObject(addrLoc, moduleName, typeName, contextInheritorModule, syntheticAltName)
{
    if (!__checkedPrivateSymbols && !__forceUsePublicSymbols)
    {
        //
        // If we haven't yet checked, do a type check in NTDLL against something we know isn't in the publics.
        //
        __usePrivateSymbols = false;
        try
        {
            var privateType = host.getModuleType(moduleName, "_IMAGE_RESOURCE_DATA_ENTRY", contextInheritorModule);
            __usePrivateSymbols = !(privateType === undefined || privateType == null);
        }
        catch(e)
        {
        }
        __checkedPrivateSymbols = true;
    }

    if (__usePrivateSymbols)
    {
        return host.createTypedObject(addrLoc, moduleName, typeName, contextInheritorModule);
    }
    else
    {
        var creatorName = typeName;
        if (!(syntheticAltName === undefined))
        {
            creatorName = syntheticAltName;
        }
        var typeClass = __globalScope["__" + creatorName];
        var result = new typeClass(addrLoc, contextInheritorModule);
        return result;
    }
}

function __getEnumerant(enumNames, val)
{
    var enumerants = Object.getOwnPropertyNames(enumNames);
    for (var enumerantName of enumerants)
    {
        if (enumNames[enumerantName] == val)
        {
            return enumerantName;
        }
    }

    return null;
}

//*************************************************
// Image Information and ELF Parsing

class __ELFImageInformation
{
    constructor(module)
    {
        this.__module = module;
        this.__ranges = [];

        //
        // Go through the program headers and figure out all file offset <-> RVA mappings
        // 
        for (var ph of this.ProgramHeaders)
        {
            var header = ph.__programHeader;
            if (header.FileSize != 0 && header.MemSize != 0 && header.FileSize == header.MemSize)
            {
                this.__ranges.push( { memoryStart: header.VAddr, fileStart: header.Offset, size: header.FileSize });
            }
        }
    }

    get ClassTypes()
    {
        return { 
            Bitness32 : 1,
            Bitness64 : 2
        };
    }
    
    get OSTypes()
    {
        return {
            SystemV: 0,
            HPUX: 1,
            NetBSD: 2,
            Linux: 3,
            GNUHurd: 4,
            Solaris: 6,
            AIX: 7,
            IRIX: 8,
            FreeBSD: 9,
            Tru64: 10,
            NovellModesto: 11,
            OpenBSD: 12,
            OpenVMS: 13,
            NonStopKernel: 13,
            AROS: 15,
            FenixOS: 16,
            CloudABI: 17,
            Sortix: 83
        };
    }

    get ImageTypes()
    {
        return {
            Relocatable: 1,
            Executable: 2,
            SharedObject: 3,
            CoreFile: 4
        };
    }

    get MachineTypes()
    {
        return {
            Undefined: 0,
            SPARC: 2,
            x86: 3,
            MIPS: 8,
            PowerPC: 20,
            S390: 22,
            ARM: 40,
            SuperH: 42,
            IA64: 50,
            x64: 62,
            AArch64: 0xB7,
            RISCV: 0xF3
        };
    }

    get ProgramHeaderTypes()
    {
        return {
            PT_NULL: 0,
            PT_LOAD: 1,
            PT_DYNAMIC: 2,
            PT_INTERP: 3,
            PT_NOTE: 4,
            PT_SHLIB: 5,
            PT_PHDR: 6,
            PT_LOOS: 0x60000000,
            GNU_EH_FRAME: 0x6474E550,
            GNU_STACK: 0x6474E551,
            GNU_RELRO: 0x6474E552,
            PT_HIOS: 0x6FFFFFFF,
            PT_LOPROC: 0x70000000,
            PT_HIPROC: 0x7FFFFFFF
        };
    }

    get DynamicTypes()
    {
        return {
            DT_NULL: 0,
            DT_NEEDED: 1,
            DT_PLTRELSZ: 2,
            DT_PLTGOT: 3,
            DT_HASH: 4,
            DT_STRTAB: 5,
            DT_SYMTAB: 6,
            DT_RELA: 7,
            DT_RELASZ: 8,
            DT_RELAENT: 9,
            DT_STRSZ: 10,
            DT_SYMENT: 11,
            DT_INIT: 12,
            DT_FINI: 13,
            DT_SONAME: 14,
            DT_RPATH: 15,
            DT_SYMBOLIC: 16,
            DT_REL: 17,
            DT_RELSZ: 18,
            DT_RELENT: 19,
            DT_PLTREL: 20,
            DT_DEBUG: 21,
            DT_TEXTREL: 22,
            DT_JMPREL: 23,
            DT_BIND_NOW: 24,
            DT_INIT_ARRAY: 25,
            DT_FINI_ARRAY: 26,
            DT_INIT_ARRAYSZ: 27,
            DT_FINI_ARRAYSZ: 28,
            DT_RUNPATH: 29,
            DT_FLAGS: 30,
            DT_ENCODING: 31,
            DT_PREINIT_ARRAY: 32,
            DT_PREINIT_ARRAYSZ: 33,
            DT_MAXPOSTAGS: 34,

            DT_SUNW_AUXILIARY: 0x6000000D,
            DT_SUNW_RTLDINF: 0x6000000E,
            DT_SUNW_FILTER: 0x6000000F,
            DT_SUNW_CAP: 0x60000010,
            DT_VALRNGLO: 0x6FFFFD00,
            DT_GNU_PRELINKED: 0x6FFFFDF5,
            DT_GNU_CONFLICTSZ: 0x6FFFFDF6,
            DT_GNU_LIBLISTSZ: 0x6FFFFDF7,
            DT_CHECKSUM: 0x6FFFFDF8,
            DT_PLTPADSZ: 0x6FFFFDF9,
            DT_MOVEENT: 0x6FFFFDFA,
            DT_MOVESZ: 0x6FFFFDFB,
            DT_FEATURE_1: 0x6FFFFDFC,
            DT_POSFLAG_1: 0x6FFFFDFD,
            DT_SYMINSZ: 0x6FFFFDFE,
            DT_SYMINENT: 0x6FFFFDFF,

            DT_GNU_HASH: 0x6FFFFEF5,
            DT_GNU_CONFLICT: 0x6FFFFEF8,
            DT_GNU_LIBLIST: 0x6FFFFEF9,
            DT_CONFIG: 0x6FFFFEFA,
            DT_DEPAUDIT: 0x6FFFFEFB,
            DT_AUDIT: 0x6FFFFEFC,
            DT_PLTPAD: 0x6FFFFEFD,
            DT_MOVETAB: 0x6FFFFEFE,
            DT_SYMINFO: 0x6FFFFEFF,
            DT_VERSYM: 0x6FFFFFF0,
            DT_RELACOUNT: 0x6FFFFFF9,
            DT_RELCOUNT: 0x6FFFFFFA,
            DT_FLAGS_1: 0x6FFFFFFB,
            DT_VERDEF: 0x6FFFFFFC,
            DT_VERDEFNUM: 0x6FFFFFFD,
            DT_VERNEED: 0x6FFFFFFE,
            DT_NVERNEEDNUM: 0x6FFFFFFF
        };
    }

    get SectionHeaderTypes()
    {
        return {
            SHT_NULL: 0,
            SHT_PROGBITS: 1,
            SHT_SYMTAB: 2,
            SHT_STRTAB: 3,
            SHT_RELA: 4,
            SHT_HASH: 5,
            SHT_DYNAMIC: 6,
            SHT_NOTE: 7,
            SHT_NOBITS: 8,
            SHT_REL: 9,
            SHT_SHLIB: 10,
            SHT_DYNSYM: 11,
            SHT_INIT_ARRAY: 14,
            SHT_FINI_ARRAY: 15,
            SHT_PREINIT_ARRAY: 16,
            SHT_GROUP: 17,
            SHT_SYMTAB_SHNDX: 18,
            SHT_NUM: 19,
            SHT_LOOS: 0x60000000
        };
    }

    get NoteTypes()
    {
        return {
            NT_ABI_TAG: 1,
            NT_PRSTATUS: 1,
            NT_GNU_HWCAP: 2,
            NT_FPREGSET: 2,
            NT_GNU_BUILD_ID: 3,
            NT_PRPSINFO: 3,
            NT_GNU_GOLD_VERSION: 4,
            NT_AUXV: 6,
            NT_PSTATUS: 10,
            NT_FPREGS: 12,
            NT_PSINFO: 13,
            NT_LWPSTATUS: 16,
            NT_LPWSINFO: 17,
            NT_FDO_BUILDINFO: 0xcafe1a7e
        };
    }

    get ELFHeader()
    {
        var header = __createSyntheticTypedObject(this.__module.BaseAddress, __keyModule, "___ELF_HEADER32", this.__module);

        if (header.Identifier[0] != 0x7F || header.Identifier[1] != 0x45 ||
            header.Identifier[2] != 0x4c || header.Identifier[3] != 0x46)
        {
            throw new Error("Unrecognized ELF header for module: " + this.__module.Name);
        }

        if (header.Class == this.ClassTypes.Bitness32)
        {
            return header;
        }
        else if (header.Class == this.ClassTypes.Bitness64)
        {
            return __createSyntheticTypedObject(this.__module.BaseAddress, __keyModule, "___ELF_HEADER64", this.__module);
        }
    }

    // GetRVAForOffset
    //
    // Gets the RVA for an offset within an ELF image.
    //
    GetRVAForOffset(offset)
    {
        for (var range of this.__ranges)
        {
            if (offset.compareTo(range.fileStart) >= 0 && offset.compareTo(range.fileStart.add(range.size)))
            {
                var delta = offset.subtract(range.fileStart);
                var addr = range.memoryStart.add(delta);
                var size = range.size.subtract(delta);
                return { vaddr: addr, size: size };
            }
        }
        return null;
    }

    // GetSectionHeaderBase():
    //
    // Gets the base address of a section header given its index.
    //
    GetSectionHeaderBase(ndx)
    {
        var elfHeader = this.ELFHeader;
        var shdrsBase = elfHeader.OffsetSHdr;

        if (ndx < 0 || ndx >= elfHeader.SHNum)
        {
            throw new RangeError("Invalid section index: " + ndx);
        }

        var shdrsVAddr = GetRVAForOffset(shdrsBase);
        if (!shdrsVAddr)
        {
            return null;
        }
        shrdsVAddr.vaddr.add(this.__module.BaseAddress);

        var entSize = elfHeader.SHEntSize;
        return shdrsVAddr.vaddr.add(entSize.multiply(ndx));
    }

    // GetSectionHeader():
    //
    // Gets the section header for a given section.  If it is not mapped (the table is often not mapped
    // into process memory), null is returned.
    //
    GetSectionHeader(ndx)
    {
        var addr = GetSectionHeaderBase(ndx);
        if (!addr)
        {
            return null;
        }

        return _createSyntheticTypedObject(addr, 
                                           __keyModule, 
                                           this.Is64Bit ? "___ELF_SECTIONHEADER64" : "___ELF_SECTIONHEADER32",
                                           this.__module);
    }

    // GetSection():
    //
    // Gets a particular section in the ELF.  If it is not mapped (the table is often not mapped
    // into process memory), null is returned.
    //
    GetSection(ndx)
    {
        var header = GetSectionHeader(ndx);
        if (!header)
        {
            return null;
        }

        // @TODO:
        return null;
    }

    get SectionNamesBase()
    {
        return this.__module.BaseAddress.add(this.ELFHeader.SHStrNdx);
    }

    get ProgramHeaders()
    {
        return new __ELFProgramHeaders(this.__module, this);
    }

    get Is64Bit()
    {
        return (this.ELFHeader.Class == this.ClassTypes.Bitness64);
    }
}

class __GNUBuildIdData
{
    constructor(note)
    {
        this.__note = note;
    }

    get BuildID()
    {
        var str = "";
        var dataBytes = this.__note.Data;
        var dataSize = this.__note.__noteHeader.DescSize;
        for (var i = 0; i < dataSize; ++i)
        {
            str += __convertHex(dataBytes[i], 8);
        }
        return str;
    }
}

class __FdoBuildInfo
{
    constructor(note)
    {
        this.__note = note;
        
        var dataBytes = this.__note.Data;
        var dataSize = this.__note.__noteHeader.DescSize;

        var jsonStr = "";
        for (var i = 0; i < dataSize; ++i)
        {
            jsonStr += String.fromCharCode(dataBytes[i]);
        }

        this.__buildInfo = JSON.parse(jsonStr);
    }

    get BuildInfo()
    {
        return this.__buildInfo;
    }
}

class __ELFNote
{
    constructor(content, noteHeader)
    {
        this.__content = content;
        this.__noteHeader = noteHeader;
    }

    toString()
    {
        var str = this.Name + " (" + this.Type + ")";
        return str;
    }

    get Name()
    {
        var nameAddr = this.__noteHeader.targetLocation.add(12);
        var name = host.memory.readString(nameAddr, this.__noteHeader.NameSize - 1);

        //
        // In the event someone put the alignment padding in the namesz and there are embedded nulls
        // in the string, strip them.  Chakra seems to return Length==<to first null terminator> with embedded nulls
        // which I don't believe is correct.  In any case, walk backwards from .NameSize to find the first non
        // null and strip.
        //
        var idx = this.__noteHeader.NameSize - 2;
        while(idx > 0 && name.charCodeAt(idx) == 0) { --idx; }
        if (idx < this.__noteHeader.NameSize - 2)
        {
            name = name.substring(0, idx + 1);
        }

        return name;
    }

    get Type()
    {
        var typeName = __getEnumerant(this.__content.__programHeader.__imageInfo.NoteTypes, this.__noteHeader.Type);
        if (!typeName)
        {
            return "Unknown (" + this.__programHeader.Type.toString(16) + ")";
        }
        return typeName;
    }

    get DataAddress()
    {
        return this.__noteHeader.targetLocation.add(12).add(this.__noteHeader.NameSize);
    }

    get Data()
    {
        var dataBytes = host.memory.readMemoryValues(this.DataAddress, this.__noteHeader.DescSize, 1, false, 
                                                     this.__content.__programHeader.__module);
        return dataBytes;
    }

    get Content()
    {
        var ntTypes = this.__content.__programHeader.__imageInfo.NoteTypes;
        if (this.Name == "GNU" && this.__noteHeader.Type == ntTypes.NT_GNU_BUILD_ID)
        {
            return new __GNUBuildIdData(this);
        }
        else if (this.Name == "FDO" && this.__noteHeader.Type == ntTypes.NT_FDO_BUILDINFO)
        {
            return new __FdoBuildInfo(this);
        }
        return undefined;
    }
}

class __ELFNotes
{
    constructor(content)
    {
        this.__content = content;
    }

    *[Symbol.iterator]()
    {
        var noteHeaderSize = 12;

        var address = this.__content.__programHeader.Address;
        var bytesRemaining = this.__content.__programHeader.Header.MemSize;
        while (bytesRemaining > noteHeaderSize)
        {
            var noteHeader = __createSyntheticTypedObject(address, __keyModule, "___ELF_NOTE", 
                                                          this.__content.__programHeader.__module);

            var totalSize = noteHeaderSize + noteHeader.NameSize + noteHeader.DescSize;
            if (bytesRemaining < totalSize)
            {
                break;
            }

            yield new __ELFNote(this.__content, noteHeader);
            address = address.add(totalSize);
            bytesRemaining = bytesRemaining.subtract(totalSize);
        }
    }
}

class __ELFNoteContent
{
    constructor(programHeader)
    {
        this.__programHeader = programHeader;
    }

    get Notes()
    {
        return new __ELFNotes(this);
    }
};

class __ELFDynamicDebugDataEntry
{
    constructor(content, linkMapEntry)
    {
        this.__content = content;
        this.__linkMapEntry = linkMapEntry;
    }

    toString()
    {
        return this.Name + " == 0x" + this.Address.toString(16);
    }

    get Address()
    {
        return this.__linkMapEntry.LAddr;
    }

    get Name()
    {
        return host.memory.readString(this.__linkMapEntry.LName, this.__content.__programHeader.__module);
    }
}

class __ELFDynamicDebugDataEntries
{
    constructor(content, dynamicEntry)
    {
        this.__content = content;
        this.__dynamicEntry = dynamicEntry;
    }

    *[Symbol.iterator]()
    {
        var is64 = this.__content.__imageInfo.Is64Bit;
        var rdebug = __createSyntheticTypedObject(this.__dynamicEntry.Val, __keyModule,
                                                  (is64 ? "___ELF_RDEBUG64" : "___ELF_RDEBUG32"),
                                                  this.__content.__programHeader.__module);

        var linkMapEntryAddr = rdebug.AddrRMap;
        while (linkMapEntryAddr != 0)
        {
            var linkMapEntry = __createSyntheticTypedObject(linkMapEntryAddr, __keyModule,
                                                            (is64 ? "___ELF_LINKMAPENTRY64" : "___ELF_LINKMAPENTRY32"),
                                                            this.__content.__programHeader.__module);

            yield new __ELFDynamicDebugDataEntry(this.__content, linkMapEntry);

            linkMapEntryAddr = linkMapEntry.LNext;
        }
    }
};

class __ELFDynamicDebugData
{
    constructor(content, dynamicEntry)
    {
        this.__content = content;
        this.__dynamicEntry = dynamicEntry;
    }

    get Entries()
    {
        return new __ELFDynamicDebugDataEntries(this.__content, this.__dynamicEntry);
    }
};

class __ELFDynamicEntry
{
    constructor(content, dynamicEntry, address)
    {
        this.__content = content;
        this.__dynamicEntry = dynamicEntry;
        this.__address = address;
    }

    toString()
    {
        var str = this.Type + " == 0x" + this.Value.toString(16);
        return str;
    }

    get Type()
    {
        var typeName = __getEnumerant(this.__content.__imageInfo.DynamicTypes, this.__dynamicEntry.Tag);
        if (!typeName)
        {
            return "Unknown (" + this.__dynamicEntry.Tag.toString(16) + ")";
        }
        return typeName;
    }

    get Value()
    {
        return this.__dynamicEntry.Val;
    }

    get Content()
    {
        if (this.__dynamicEntry.Tag == this.__content.__imageInfo.DynamicTypes.DT_DEBUG)
        {
            return new __ELFDynamicDebugData(this.__content, this.__dynamicEntry);
        }
    }
};

class __ELFDynamicEntries
{
    constructor(content)
    {
        this.__content = content;
    }

    *[Symbol.iterator]()
    {
        var is64 = this.__content.__imageInfo.Is64Bit;
        var dynamicEntrySize = (is64 ? 16 : 8);

        var address = this.__content.__programHeader.Address;
        var bytesRemaining = this.__content.__programHeader.Header.MemSize;
        while (bytesRemaining > dynamicEntrySize)
        {
            var dynamicEntry = __createSyntheticTypedObject(address, __keyModule,
                                                            (is64 ? "___ELF_DYNAMICENTRY64" : "___ELF_DYNAMICENTRY32"),
                                                            this.__content.__programHeader.__module);

            yield new __ELFDynamicEntry(this.__content, dynamicEntry, address);
            address = address.add(dynamicEntrySize);
            bytesRemaining = bytesRemaining.subtract(dynamicEntrySize);
        }
    }
}

class __ELFDynamicContent
{
    constructor(programHeader, imageInfo)
    {
        this.__programHeader = programHeader;
        this.__imageInfo = imageInfo;
    }

    get Entries()
    {
        return new __ELFDynamicEntries(this);
    }
};

class __ELFProgramHeader
{
    constructor(module, imageInfo, programHeader)
    {
        this.__module = module;
        this.__imageInfo = imageInfo;
        this.__programHeader = programHeader;
    }

    toString()
    {
        var str = this.Type;

        var fileSz = this.__programHeader.FileSize;
        var memSz = this.__programHeader.MemSize;
        if (fileSz != 0 && memSz != 0)
        {
            var fileBegin = this.__programHeader.Offset;
            var fileEnd = fileBegin.add(fileSz);
            var memBegin = this.__programHeader.VAddr;
            var memEnd = memBegin.add(memSz);

            str += " { File : [" + fileBegin + ", " + fileEnd + ") Memory : [" + memBegin + ", " + memEnd + ") }";
        }

        return str;
    }

    get Type()
    {
        var typeName = __getEnumerant(this.__imageInfo.ProgramHeaderTypes, this.__programHeader.Type);
        if (!typeName)
        {
            return "Unknown (" + this.__programHeader.Type.toString(16) + ")";
        }
        return typeName;
    }

    get Address()
    {
        if (this.__programHeader.MemSize == 0)
        {
            return undefined;
        }

        return this.__module.BaseAddress.add(this.__programHeader.VAddr);
    }

    get Header()
    {
        return this.__programHeader;
    }

    get Content()
    {
        var phTypes = this.__imageInfo.ProgramHeaderTypes;
        if (this.__programHeader.Type == phTypes.PT_NOTE)
        {
            return new __ELFNoteContent(this, this.__imageInfo);
        }
        else if (this.__programHeader.Type == phTypes.PT_DYNAMIC)
        {
            return new __ELFDynamicContent(this, this.__imageInfo);
        }
    }
}

// __ELFProgramHeaders
//
// Enumerator for program headers in a mapped ELF
//
class __ELFProgramHeaders
{
    constructor(module, imageInfo)
    {
        this.__module = module;
        this.__imageInfo = imageInfo;
    }

    *[Symbol.iterator]()
    {
        var header = this.__imageInfo.ELFHeader;
        var is64 = this.__imageInfo.Is64Bit;

        var entryCount = header.PHNum;
        var entrySize = header.PHEntSize;
        var offset = header.OffsetPHdr;

        while (entryCount > 0)
        {
            var programHeader = __createSyntheticTypedObject(this.__module.BaseAddress.add(offset), __keyModule,
                                                             is64 ? "___ELF_PROGRAMHEADER64" : "___ELF_PROGRAMHEADER32",
                                                             this.__module);

            yield new __ELFProgramHeader(this.__module, this.__imageInfo, programHeader);

            entryCount--;
            offset = offset.add(entrySize);
        }
    }
}

//*************************************************
// Image Information and PE Parsing

function __identifyImageType(module)
{
    var magic = host.memory.readMemoryValues(module.BaseAddress, 4, 1, false, module);
    if (magic[0] == 0x4D && magic[1] == 0x5a)
    {
        return "PE";
    }
    else if (magic[0] == 0x7F && magic[1] == 0x45 && magic[2] == 0x4C && magic[3] == 0x46)
    {
        return "ELF";
    }

    return null;
}

function __padToDword(offset32)
{
    return (offset32 + 3) & ~3;
}

// __MultiDescriptor:
//
// A class which abstracts away a set of descriptors.
//
class __MultiDescriptor
{
    constructor(module, entry, typeName)
    {
        this.__module = module;
        this.__entry = entry;
        this.__typeName = typeName;
    }

    *[Symbol.iterator]()
    {
        if (this.__entry.VirtualAddress != 0)
        {
            var address = this.__module.BaseAddress.add(this.__entry.VirtualAddress);
            var descriptor = __createTypedObject(address, __keyModule, this.__typeName, this.__module);
            var descriptorSize = descriptor.targetSize;
            var count = this.__entry.Size / descriptorSize;
            for (var i = 0; i < count; ++i)
            {
                yield descriptor;
                address += descriptorSize;
                descriptor = __createTypedObject(address, __keyModule, this.__typeName, this.__module);
            }
        }
    }
}

// __PEImageInformation:
//
// A class which abstracts image information for a PE image.
//
class __PEImageInformation
{
    constructor(module)
    {
        this.__module = module;
    }
        
    get DirectoryNumbers()
    {
        return { 
            exportDirectory : 0,
            importDirectory : 1,
            resourceDirectory : 2,
            exceptionDirectory : 3,
            securityDirectory : 4,
            baseRelocationDirectory : 5,
            debugDirectory : 6,
            architectureDirectory : 7,
            globalPointerDirectory : 8,
            TLSDirectory : 9,
            loadConfigurationDirectory : 10,
            boundImportDirectory : 11,
            iatDirectory : 12,
            delayImportDirectory : 13,
            COMRuntimeDescriptorDirectory : 14
        };
    }

    get ImageHeader()
    {
        var header = __createTypedObject(this.__module.BaseAddress, __keyModule, "_IMAGE_DOS_HEADER", this.__module);
        if (header.e_magic != 0x5A4D)
        {
            throw new Error("Unrecognized image header for module: " + this.__module.Name);
        }
        return header;
    }
    
    get FileHeader()
    {
        var imageHeader = this.ImageHeader;
        var offset = imageHeader.e_lfanew;
        var fileOffset = this.__module.BaseAddress.add(offset);
        var moduleName = this.__module.__ComparisonName;
        var signaturePointer = host.createPointerObject(fileOffset, moduleName, "unsigned long *", this.__module);
        var signature = signaturePointer.dereference();
        if (signature != 0x00004550)
        {
            throw new Error("Unrecognized file header for module: " + this.__module.Name);
        }
        fileOffset = fileOffset.add(4); // +sizeof(ULONG) -- PE signature
        var header = __createTypedObject(fileOffset, __keyModule, "_IMAGE_FILE_HEADER", this.__module);
        return header;
    }
    
    get OptionalHeader()
    {
        var fileHeader = this.FileHeader;
        var fileHeaderLocation = fileHeader.targetLocation;
        var fileHeaderSize = fileHeader.targetSize;
        var optionalHeaderLocation = fileHeaderLocation.add(fileHeaderSize);
        var optionalHeader = __createTypedObject(optionalHeaderLocation, __keyModule, "_IMAGE_OPTIONAL_HEADER", this.__module, "_IMAGE_OPTIONAL_HEADER32");
        if (optionalHeader.Magic == 0x010B)
        {
            // PE32 (optionalHeader is correct)

        }
        else if (optionalHeader.Magic == 0x020B)
        {
            // PE32+ (optionalHeader is incorrect!)
            optionalHeader = __createTypedObject(optionalHeaderLocation, __keyModule, "_IMAGE_OPTIONAL_HEADER64", this.__module);
        }
        else
        {
            throw new Error("Unrecognized optional header for module: " + this.__module.Name);
        }
        return optionalHeader;
    }

    get IsPE32()
    {
        return this.OptionalHeader.Magic == 0x010B;
    }
    
    get Directories()
    {
        var optionalHeader = this.OptionalHeader;
        return optionalHeader.DataDirectory;
    }
    
    get ResourceDirectory()
    {
        var entry = this.Directories[this.DirectoryNumbers.resourceDirectory];
        if (entry.VirtualAddress == 0)
        {
            //
            // It does not exist!
            //
            return undefined;
        }
        var address = this.__module.BaseAddress.add(entry.VirtualAddress);
        return __createTypedObject(address, __keyModule, "_IMAGE_RESOURCE_DIRECTORY", this.__module);
    }

    get ImportDescriptor()
    {
        var entry = this.Directories[this.DirectoryNumbers.importDirectory];
        if (entry.VirtualAddress == 0)
        {
            //
            // It does not exist
            //
            return undefined;
        }
        var address = this.__module.BaseAddress.add(entry.VirtualAddress);
        return __createTypedObject(address, __keyModule, "_IMAGE_IMPORT_DESCRIPTOR", this.__module);
    }

    get DelayImportDescriptor()
    {
        var entry = this.Directories[this.DirectoryNumbers.delayImportDirectory];
        if (entry.VirtualAddress == 0)
        {
            //
            // It does not exist
            //
            return undefined;
        }
        var address = this.__module.BaseAddress.add(entry.VirtualAddress);
        return __createTypedObject(address, __keyModule, "_IMAGE_DELAYLOAD_DESCRIPTOR", this.__module);
    }

    get ExportDescriptor()
    {
        var entry = this.Directories[this.DirectoryNumbers.exportDirectory];
        if (entry.VirtualAddress == 0)
        {
            //
            // It does not exist
            //
            return undefined;
        }
        var address = this.__module.BaseAddress.add(entry.VirtualAddress);
        return __createTypedObject(address, __keyModule, "_IMAGE_EXPORT_DIRECTORY", this.__module);
    }

    get DebugDescriptors()
    {
        var entry = this.Directories[this.DirectoryNumbers.debugDirectory];
        if (entry.VirtualAddress == 0)
        {
            //
            // It does not exist
            //
            return undefined;
        }
        return new __MultiDescriptor(this.__module, entry, "_IMAGE_DEBUG_DIRECTORY");
    }

    get IATAddress()
    {
        var entry = this.Directories[this.DirectoryNumbers.iatDirectory];
        if (entry.VirtualAddress == 0)
        {
            //
            // It does not exist
            //
            return undefined;
        }
        var address = this.__module.BaseAddress.add(entry.VirtualAddress);
        return address;
    }
}

class __ImageDirectories
{
    constructor(descModule, imageInfo)
    {
        this.__descModule = descModule;
        this.__imageInfo = imageInfo;
    }

    get ExportDirectory()
    {
        return this.__imageInfo.ExportDescriptor;
    }

    get ImportDirectory()
    {
        return this.__imageInfo.ImportDescriptor;
    }

    get ResourceDirectory()
    {
        return this.__imageInfo.ResourceDirectory;
    }

    get DelayImportDirectory()
    {
        return this.__imageInfo.DelayImportDescriptor;
    }

    get DebugDirectories()
    {
        return this.__imageInfo.DebugDescriptors;
    }
}

// __ImageInformation:
//
// A class which abstracts image information for an image (PE or otherwise)
//
class __ImageInformation
{
    constructor(module)
    {
        this.__module = module;

        //
        // Take a look at the header.  Do we recognize the format.
        //
        var imageType = __identifyImageType(module);
        if (imageType == null)
        {
            throw new Error("Unrecognized image format");
        }

        this.ImageType = imageType;

        if (imageType == "PE")
        {
            this.Information = new __PEImageInformation(module);
        }
        else if (imageType == "ELF")
        {
            this.Information = new __ELFImageInformation(module);
        }
        else
        {
            throw new Error("Unsupported image format");
        }
    }
}

// __ImageHeaders:
//
// Class which abstracts the collection of PE image headers.
//
class __ImageHeaders
{
    constructor(descModule, imageInfo)
    {
        this.__descModule = descModule;
        this.__imageInfo = imageInfo;
    }

    get ImageHeader()
    {
        return this.__imageInfo.ImageHeader;
    }

    get FileHeader()
    {
        return this.__imageInfo.FileHeader;
    }

    get OptionalHeader()
    {
        return this.__imageInfo.OptionalHeader;
    }
}

// __findRelatedModule:
//
// From the context of a given source module, find a module in the same context.  This module can be found
// either via its name (less optimal) or via a bound address within the module.
//
function __findRelatedModule(srcModule, relatedName, boundAddress)
{
    //
    // Two ways this can work.  If we have a bound address within the related module, that is very easy
    // to find the module.  If we do not (or it is zero), we will resolve by name (which is a far poorer choice)
    //
    var modCtx = srcModule.hostContext;
    var modSession = host.namespace.Debugger.Sessions.getValueAt(modCtx);
    var modProcess = modSession.Processes.getValueAt(modCtx);
    var modMatches = undefined;

    if (boundAddress === undefined || boundAddress.compareTo(0) == 0)
    {
        var matchName = relatedName;
        modMatches = modProcess.Modules.Where(function (mod) {
            //
            // Make sure we don't inadvertantly get any path elements.  This really should just be the name.
            //
            var modName = mod.__ComparisonName;
            return modName.toLowerCase() == matchName.toLowerCase();
        });
    }
    else
    {
        modMatches = modProcess.Modules.Where(function (mod) {
            var low = boundAddress.compareTo(mod.BaseAddress);
            var high = boundAddress.compareTo(mod.BaseAddress.add(mod.Size));
            return (low >= 0 && high < 0);
        });
    }

    if (modMatches.Count() == 1)
    {
        return modMatches.First();
    }
    else
    {
        return undefined;
    }    
}

//*******************************************************************************
// Resource Table Implementation:
//

// __ResourceEntry:
//
// Represents a single entry in an image's resource table.
//
class __ResourceEntry
{
    constructor(resourceModule, resourceDirectory, directoryEntry, resourceEntry, priorPath, rootIdx)
    {
        this.__resourceModule = resourceModule;
        this.__resourceDirectory = resourceDirectory;
        this.__directoryEntry = directoryEntry;
        this.__resourceEntry = resourceEntry;
        this.__priorPath = priorPath;
        this.__rootIdx = rootIdx;

        if (this.__rootIdx === undefined)
        {
            this.__rootIdx = this.__Index;
        }
    }
    
    toString()
    {
        var baseDesc = "Resource '" + this.Path + "'";
        if (this.Type !== undefined)
        {
            baseDesc += " (" + this.Type + ")";
        }
        return baseDesc;
    }
    
    get Path()
    {
        var pathElement;
        
        if (this.__IsNamed)
        {
            pathElement = this.Name;
        }
        else
        {
            pathElement = this.Id.toString(10);
        }

        if (this.__priorPath == undefined)
        {
            return pathElement;
        }        
        else
        {
            return this.__priorPath + "/" + pathElement;
        }
    }

    get Type()
    {
        if (this.__rootIdx === undefined || typeof this.__rootIdx === 'string')
        {
            return undefined;
        }

        var typeName = __resourceTypes.getNameForType(this.__rootIdx);
        if (typeName == null)
        {
            return undefined;
        }

        return typeName;
    }
    
    get __IsNamed()
    {
        return this.__resourceEntry.NameIsString != 0;
    }
    
    get __HasChildren()
    {
        return this.__resourceEntry.DataIsDirectory != 0;
    }

    get __Index()
    {
        if (this.__IsNamed)
        {
            return this.Name;
        }
        else
        {
            return this.Id;
        }
    }
    
    get Name()
    {
        if (this.__IsNamed)
        {
            var nameLocation = this.__resourceDirectory.targetLocation.add(this.__resourceEntry.NameOffset);
            var nameData = __createTypedObject(nameLocation, __keyModule, "_IMAGE_RESOURCE_DIR_STRING_U", this.__resourceModule);
            var nameLocation = nameData.NameString.targetLocation;
            //
            // @TODO: We have no easy way to read a non-null terminated string from the
            //        target address space!
            //
            var nameString = host.memory.readWideString(nameLocation, this.__resourceModule);
            return nameString.slice(0, nameData.Length);
        }
        return undefined;
    }
    
    get Id()
    {
        if (this.__IsNamed)
        {
            return undefined;
        }
        else
        {
            return this.__resourceEntry.NameOffset;
        }
    }
    
    get Children()
    {
        if (this.__HasChildren)
        {
            var dataLocation = this.__resourceDirectory.targetLocation.add(this.__resourceEntry.OffsetToDirectory);
            var childDirectory = __createTypedObject(dataLocation, __keyModule, "_IMAGE_RESOURCE_DIRECTORY", this.__resourceModule);
            return new __ResourceTable(this.__resourceModule, this.__resourceDirectory, childDirectory, this.Path, this.__rootIdx);
        }
        else
        {
            return undefined;
        }
    }

    get DataAddress()
    {
        if (this.__HasChildren)
        {
            return undefined;
        }
        else
        {
            var dataEntryLocation = this.__resourceDirectory.targetLocation.add(this.__resourceEntry.OffsetToData);
            var dataEntry = __createTypedObject(dataEntryLocation, __keyModule, "_IMAGE_RESOURCE_DATA_ENTRY", this.__resourceModule);
            var dataAddress = this.__resourceModule.BaseAddress.add(dataEntry.OffsetToData);
            return dataAddress;
        }
    }
   
    get Data()
    {
        if (this.__HasChildren)
        {
            return undefined;
        }
        else
        {
            var dataEntryLocation = this.__resourceDirectory.targetLocation.add(this.__resourceEntry.OffsetToData);
            var dataEntry = __createTypedObject(dataEntryLocation, __keyModule, "_IMAGE_RESOURCE_DATA_ENTRY", this.__resourceModule);
            var dataAddress = this.__resourceModule.BaseAddress.add(dataEntry.OffsetToData);

            var dataBlob = host.memory.readMemoryValues(dataAddress, dataEntry.Size);
            return dataBlob;
        }
    }

    get DataValue()
    {
        if (this.__HasChildren || this.__rootIdx === undefined)
        {
            return undefined;
        }

        var idx = this.__rootIdx;
        if (typeof idx != 'string')
        {
            switch(idx)
            {
                case 16:        // RT_VERSION:
                {
                    return new __VersionResourceEntry(this.__resourceModule, 
                                                      this.DataAddress, 
                                                      this.Data, 
                                                      this.Data.length, 
                                                      0, 
                                                      "");
                }

                case 24:        // RT_MANIFEST:
                {
                    return new __StringResource(this.Data,
                                                this.Data.length,
                                                false);
                }
            }
        }
        else
        {
            if (idx == "XML" || idx == "XSD" || idx == "REGISTRY" || idx == "REGINST")
            {
                return new __StringResource(this.Data,
                                            this.Data.length,
                                            false);
            }
        }

        return undefined;
    }
}

// __ResourceTypeInfo:
//
// Describes the types of resources.
//
class __ResourceTypeInfo
{
    constructor()
    {
        this.NameIndex = [null,                 // 0
                          "RT_CURSOR",          // 1
                          "RT_BITMAP",          // 2
                          "RT_ICON",            // 3
                          "RT_MENU",            // 4
                          "RT_DIALOG",          // 5
                          "RT_STRING",          // 6
                          "RT_FONTDIR",         // 7
                          "RT_FONT",            // 8
                          "RT_ACCELERATOR",     // 9
                          "RT_RCDATA",          // 10
                          "RT_MESSAGETABLE",    // 11
                          "RT_GROUP_CURSOR",    // 12
                          null,                 // 13
                          "RT_GROUP_ICON",      // 14
                          null,                 // 15
                          "RT_VERSION",         // 16
                          "RT_DLGINCLUDE",      // 17
                          null,                 // 18
                          "RT_PLUGPLAY",        // 19
                          "RT_VXD",             // 20
                          "RT_ANICURSOR",       // 21
                          "RT_ANIICON",         // 22
                          "RT_HTML",            // 23
                          "RT_MANIFEST"];       // 24

        //
        // Forward index:
        //
        this.RT_CURSOR = 1;
        this.BT_BITMAP = 2;
        this.RT_ICON = 3;
        this.RT_MENU = 4;
        this.RT_DIALOG = 5;
        this.RT_STRING = 6;
        this.RT_FONTDIR = 7;
        this.RT_FONT = 8;
        this.RT_ACCELERATOR = 9;
        this.RT_RCDATA = 10;
        this.RT_MESSAGETABLE = 11;
        this.RT_GROUP_CURSOR = 12;
        this.RT_GROUP_ICON = 14;
        this.RT_VERSION = 16;
        this.RT_DLGINCLUDE = 17;
        this.RT_PLUGPLAY = 19;
        this.RT_VXD = 20;
        this.RT_ANICURSOR = 21;
        this.RT_ANIICON = 22;
        this.RT_HTML = 23;
        this.RT_MANIFEST = 24;
    }

    getNameForType(idx)
    {
        if (idx >= this.NameIndex.length)
        {
            return null;
        }
        return this.NameIndex[idx];
    }
};

var __resourceTypes = new __ResourceTypeInfo();

// __ResourceTable:
//
// An abstraction over a resource directory (or sub-directory) within an image.
//
class __ResourceTable
{
    constructor(resourceModule, resourceDirectory, directoryEntry, priorPath, rootIdx)
    {
        this.__resourceModule = resourceModule;
        this.__resourceDirectory = resourceDirectory;
        this.__directoryEntry = directoryEntry;
        this.__priorPath = priorPath;
        this.__rootIdx = rootIdx;
    }
    
    getDimensionality()
    {
        return 1;
    }
    
    getValueAt(idx)
    {
        var isString = (typeof idx === 'string');
        for (var idxEntry of this)
        {
            var entry = idxEntry.value;
            if (isString)
            {
                if (entry.Name == idx)
                {
                    return entry;
                }
            }
            else
            {
                if (entry.Id == idx)
                {
                    return entry;
                }
            }
        }
        
        throw new RangeError("Unable to find specified resource: " + idx);
    }
    
    *[Symbol.iterator]()
    {
        var entryStart = this.__directoryEntry.targetLocation.add(this.__directoryEntry.targetSize);
        var count = this.__directoryEntry.NumberOfNamedEntries + this.__directoryEntry.NumberOfIdEntries;
        var entryLoc = entryStart;

        for (var cur = 0; cur < count; ++cur)
        {
            var resourceInfo = __createTypedObject(entryLoc, __keyModule, "_IMAGE_RESOURCE_DIRECTORY_ENTRY", this.__resourceModule);
            var resourceEntry = new __ResourceEntry(this.__resourceModule, this.__resourceDirectory, this.__directoryEntry, resourceInfo, this.__priorPath, this.__rootIdx);
            
            if (resourceEntry.__IsNamed)
            {
                yield new host.indexedValue(resourceEntry, [resourceEntry.Name]);
            }
            else
            {
                yield new host.indexedValue(resourceEntry, [resourceEntry.Id]);
            }

            entryLoc = entryLoc.add(resourceInfo.targetSize);
        }
    }
}

class __StringResource
{
    constructor(blobData, blobSize, isUnicode)
    {
        this.__blobData = blobData;
        this.__blobSize = blobSize;
        this.__isUnicode = isUnicode;
        this.__size = (this.__isUnicode ? 2 : 1);
    }

    *[Symbol.iterator]()
    {
        var curIdx = 0;
        var remaining = this.__blobSize;
        var str = "";
        while(remaining > 0)
        {
            var cc = this.__next(curIdx);
            curIdx += this.__size;
            remaining -= this.__size;
            if (cc == 10) // '\n'
            {
                yield str;
                str = "";
            }
            else if (cc != 13) // '\r'
            {
                str += String.fromCharCode(cc);
            }
        }

        if (str.length != 0)
        {
            yield str;
        }
    }

    __next(idx)
    {
        if (this.__isUnicode)
        {
            return (this.__blobData[idx] | (this.__blobData[idx + 1] << 8));
        }
        else
        {
            return this.__blobData[idx];
        }
    }
}

// __VersionResourceChildren:
//
// Parses the children of a version resource.
//
class __VersionResourceChildren
{
    constructor(entry)
    {
        this.__entry = entry;
        this.__firstChildBufferOffset = __padToDword(this.__entry.__dataOffset + this.__entry.__dataSize);
        this.__firstChildEntryOffset = this.__firstChildBufferOffset - this.__entry.__entryOffset;
        this.__entrySize = this.__entry.__entrySize;
    }

    getDimensionality()
    {
        return 1;
    }
    
    getValueAt(idx)
    {
        var isString = (typeof idx === 'string');
        if (!isString)
        {
            throw new RangeError("Invalid index: " + idx.toString());
        }

        for (var idxEntry of this)
        {
            var entry = idxEntry.value;
            if (entry.Name == idx)
            {
                return entry;
            }
        }
        
        throw new RangeError("Unable to find specified entry: " + idx);
    }

    *[Symbol.iterator]()
    {
        var bufferOffset = this.__firstChildBufferOffset;
        var remainingSize = (this.__entrySize - this.__firstChildEntryOffset);
        while (remainingSize > 8)
        {
            var child = new __VersionResourceEntry(this.__entry.__rscModule, this.__entry.__bufferAddress, 
                                                   this.__entry.__blobData, this.__entry.__blobSize, 
                                                   bufferOffset, this.__entry.Path);
            yield new host.indexedValue(child, [child.Name]);
            var childSize = child.__entrySize;
            if (childSize > remainingSize)
            {
                break;
            }
            bufferOffset += childSize;
            var alignedBufferOffset = __padToDword(bufferOffset);
            var alignment = alignedBufferOffset - bufferOffset;
            bufferOffset = alignedBufferOffset;
            remainingSize -= childSize + alignment;
        }
    }
}

// VersionResourceEntry:
//
// A single entry in the version tree of the resource table.
//
class __VersionResourceEntry
{
    //
    // A version resource is defined by:
    //
    //     WORD <total length>
    //     WORD <value length>
    //     WORD <type>
    //     STRING <key>
    //     DWORD padded to VALUE
    //     <children (each a version resource bock)>
    //
    constructor(rscModule, bufferAddress, blobData, blobSize, entryOffset, priorPath)
    {
        this.__rscModule = rscModule;
        this.__bufferAddress = bufferAddress;
        this.__blobData = blobData;
        this.__blobSize = blobSize;
        this.__entryOffset = entryOffset;
        this.__priorPath = priorPath;
    }

    toString()
    {
        return this.Path;
    }

    get Offset()
    {
        return this.__entryOffset;
    }

    get Name()
    {
        return this.__readString(this.__entryOffset + 6);
    }

    get VersionInfo()
    {
        if (this.Path == "VS_VERSION_INFO" && this.__isBinary)
        {
            var versionInfo = __createTypedObject(this.__bufferAddress.add(this.__dataOffset), __keyModule, "VS_FIXEDFILEINFO", this.__rscModule);
            var isSynthetic = (versionInfo.targetType === undefined);
            if (isSynthetic)
            {
                //
                // This is on symbols which don't have VS_FIXEDFILEINFO.  The visualizer will not work.  Attach it manually.
                // Yes -- it's not a visualizer -- but at least the fields will be there.
                //
                Object.setPrototypeOf(versionInfo, __FixedVersionInfo.prototype);
            }
            return versionInfo;
        }
        else
        {
            return undefined;
        }
    }

    get Path()
    {
        if (this.__priorPath.length == 0)
        {
            return this.Name;
        }
        else
        {
            return this.__priorPath + "\\" + this.Name;
        }
    }

    get EntryKind()
    {
        if (this.__isBinary)
        {
            return "Binary";
        }
        else
        {
            return "Text";
        }
    }

    get DataAddress()
    {
        return this.__bufferAddress.add(this.__dataOffset);
    }

    get Data()
    {
        if (this.__isBinary && this.__dataSize != 0)
        {
            return host.memory.readMemoryValues(this.DataAddress, this.__dataSize, 1, false, this.__rscModule);
        }
        else
        {
            return undefined;
        }
    }

    get Text()
    {
        if (this.__isBinary || this.__dataSize == 0)
        {
            return undefined;
        }
        else
        {
            return this.__readString(this.__dataOffset);
        }
    }

    get Children()
    {
        return new __VersionResourceChildren(this);
    }

    //*************************************************
    // Internal Utility:
    //

    get __entrySize()
    {
        return this.__readWord(this.__entryOffset);
    }

    get __dataSize()
    {
        var size = this.__readWord(this.__entryOffset + 2);
        if (!this.__isBinary)
        {
            // It's unicode characters.  Convert to bytes.
            size *= 2;
        }
        return size;
    }

    get __dataOffset()
    {
        var nameLen = (this.Name.length + 1) * 2;   // UTF-16
        var dataOffset = __padToDword(this.__entryOffset + 6 + nameLen);
        return dataOffset;
    }

    get __isBinary()
    {
        return this.__readWord(this.__entryOffset + 4) == 0;
    }

    __readByte(offset)
    {
        return this.__blobData[offset];
    }

    __readWord(offset)
    {
        return this.__blobData[offset] | (this.__blobData[offset + 1] << 8);
    }

    __readDWord(offset)
    {
        return this.__blobData[offset] | (this.__blobData[offset + 1] << 8) | (this.__blobData[offset + 2] << 16) |
               (this.__blobData[offset + 3] << 24);
    }

    __readString(offset)
    {
        var str = "";
        var cur = offset;

        while(true)
        {
            var code = this.__readWord(cur);
            if (code == 0)
            {
                break;
            }
            str += String.fromCharCode(code);
            cur += 2;
        }

        return str;
    }
}

//*******************************************************************************
// Import Table Implementation
//

// __NamedImport:
//
// Abstraction over a named import in the IAT.
//
class __NamedImport
{
    constructor(importModule, thunkEntry, boundThunkEntry)
    {
        this.__importModule = importModule;
        this.__thunkEntry = thunkEntry;
        this.__boundThunkEntry = boundThunkEntry;
    }

    toString()
    {
        return "Named Import of '" + this.ImportName +"'";
    }

    get ImportName()
    {
        // +2 == ignore the thunk hint
        var address = this.__importModule.BaseAddress.add(this.__thunkEntry).add(2);
        return host.memory.readString(address, this.__importModule);
    }

    get BoundFunctionAddress()
    {
        if (this.__boundThunkEntry === undefined)
        {
            return undefined;
        }
        return this.__boundThunkEntry;
    }

    get BoundFunction()
    {
        if (this.__boundThunkEntry === undefined)
        {
            return undefined;
        }
        return host.getModuleSymbol(this.__boundThunkEntry, this.__importModule);
    }    
}

// __OrdinalImport:
//
// Abstraction over an ordinal import in the IAT.
//
class __OrdinalImport
{
    constructor(importModule, thunkEntry, boundThunkEntry)
    {
        this.__importModule = importModule;
        this.__thunkEntry = thunkEntry;
        this.__boundThunkEntry = boundThunkEntry;
    }

    toString()
    {
        return "Ordinal Import of #" + this.OrdinalNumber; 
    }

    get OrdinalNumber()
    {
        var mask = new host.Int64(0xFFFFFFFF, 0x7FFFFFFF);
        return this.__thunkEntry.bitwiseAnd(mask);
    }

    get BoundFunctionAddress()
    {
        if (this.__boundThunkEntry === undefined)
        {
            return undefined;
        }
        return this.__boundThunkEntry;
    }

    get BoundFunction()
    {
        if (this.__boundThunkEntry === undefined)
        {
            return undefined;
        }
        return host.getModuleSymbol(this.__boundThunkEntry, this.__importModule);
    }
}

// __BoundImport:
//
// Abstraction over a bound import.
//
class __BoundImport
{
    constructor(importModule, thunkEntry)
    {
        this.__importModule = importModule;
        this.__thunkEntry = thunkEntry;
    }

    toString()
    {
        return "Bound Import of Function At " + this.__thunkEntry.toString(16);
    }

    get BoundFunctionAddress()
    {
        return this.__thunkEntry;
    }

    get BoundFunction()
    {
        return host.getModuleSymbol(this.BoundFunctionAddress, this.__importModule);
    }
}

// __ImportFunctions:
//
// An abstraction over an unbound thunk list for the IAT.
//
class __ImportFunctions
{
    constructor(importModule, thunkTable, boundThunkTable)
    {
        this.__importModule = importModule;
        this.__thunkTable = thunkTable;
        this.__boundThunkTable = boundThunkTable;
    }

    *[Symbol.iterator]()
    {
        if (this.__thunkTable === undefined)
        {
            return;
        }

        var size = (this.__importModule.__IsPE32) ? 4 : 8;
        var thunkTable = this.__thunkTable;
        var boundThunkTable = this.__boundThunkTable;
        while(true)
        {
            var thunkEntry = host.memory.readMemoryValues(thunkTable, 1, size, false, this.__importModule);
            if (thunkEntry[0].compareTo(0) == 0)
            {
                break;
            }

            var boundThunkEntry = undefined;
            if (!(boundThunkTable === undefined))
            {
                //
                // This is unreadable on a "-z"
                //
                try
                {
                    boundThunkEntry = host.memory.readMemoryValues(boundThunkTable, 1, size, false, this.__importModule)[0];
                }
                catch(exc)
                {
                    boundThunkEntry = undefined;
                }
            }

            //
            // Ensure the thunk isn't bound.
            //
            var lowMask = (size == 4) ? 0x7FFFFFFF : new host.Int64(0xFFFFFFFF, 0x7FFFFFFF);
            var isNamed = (size == 4) ? ((thunkEntry[0] & 0x80000000) == 0) : ((thunkEntry[0].getHighPart() & 0x80000000) == 0);
            var noHighThunk = thunkEntry[0].bitwiseAnd(lowMask);
            var moduleEnd = this.__importModule.BaseAddress.add(this.__importModule.Size).subtract(1);
            var rva = this.__importModule.BaseAddress.add(noHighThunk);
            
            if (rva.compareTo(moduleEnd) > 0)
            {
                yield new __BoundImport(this.__importModule, thunkEntry[0]);
            }
            else if (isNamed)
            {
                yield new __NamedImport(this.__importModule, thunkEntry[0], boundThunkEntry)
            }
            else
            {
                yield new __OrdinalImport(this.__importModule, thunkEntry[0], boundThunkEntry);
            }

            thunkTable = thunkTable.add(size);
            if (!(boundThunkTable === undefined))
            {
                boundThunkTable = boundThunkTable.add(size);
            }
        }
    }
}

// __ImportEntry:
//
// An abstraction over a particular DLL import entry within the IAT.
//
class __ImportEntry
{
    constructor(importModule, entry)
    {
        this.__importModule = importModule;
        this.__entry = entry;
    }

    toString()
    {
        return this.ModuleName;
    }

    get __FirstBoundThunk()
    {
        var size = this.__importModule.__IsPE32 ? 4 : 8;
        var thunkAddr = this.__importModule.BaseAddress.add(this.__entry.FirstThunk);
        var originalThunkAddr = this.__importModule.BaseAddress.add(this.__entry.OriginalFirstThunk);
        var thunk = undefined;
        try
        {
            thunk = host.memory.readMemoryValues(thunkAddr, 1, size, false, this.__importModule);
        }
        catch(exc)
        {
            return undefined;
        }
        var originalThunk = host.memory.readMemoryValues(originalThunkAddr, 1, size, false, this.__importModule);
        if (thunk[0].compareTo(originalThunk[0]) == 0)
        {
            //
            // If the values are identical, the IAT isn't bound.  Don't return anything.
            //
            return undefined;
        }
        return thunk[0];
    }

    get ModuleName()
    {
        var address = this.__importModule.BaseAddress.add(this.__entry.Name);
        return host.memory.readString(address, this.__importModule);
    }

    get ResolvedModule()
    {
        return __findRelatedModule(this.__importModule, this.ModuleName, this.__FirstBoundThunk);
    }

    get Functions()
    {
        var boundThunkTable = this.__importModule.BaseAddress.add(this.__entry.FirstThunk);
        var thunkTable = this.__importModule.BaseAddress.add(this.__entry.OriginalFirstThunk);
        return new __ImportFunctions(this.__importModule, thunkTable, boundThunkTable);
    }
}

// __ImportsTable:
//
// An abstraction over the IAT within an image.
//
class __ImportsTable
{
    constructor(importModule, startingEntry)
    {
        this.__importModule = importModule;
        this.__startingEntry = startingEntry;
    }

    getDimensionality()
    {
        return 1;
    }
    
    getValueAt(idx)
    {
        for (var idxEntry of this)
        {
            var entry = idxEntry.value;
            if (entry.ModuleName == idx)
            {
                return entry;
            }
        }
        
        throw new RangeError("Unable to find specified import: " + idx);
    }
    
    *[Symbol.iterator]()
    {
        if (this.__startingEntry === undefined)
        {
            return;
        }

        var currentEntry = this.__startingEntry;
        var size = currentEntry.targetSize;
        while (currentEntry.Name != 0)
        {
            var entry = new __ImportEntry(this.__importModule, currentEntry);
            yield new host.indexedValue(entry, [entry.ModuleName]);
            currentEntry = __createTypedObject(currentEntry.targetLocation.add(size), __keyModule, "_IMAGE_IMPORT_DESCRIPTOR", this.__importModule);
        }
    }
}

//*******************************************************************************
// DelayLoad Import Table Implementation
//

// __DelayImportEntry:
//
// An abstraction over a particular DLL delay import entry within the delay import table.
//
class __DelayImportEntry
{
    constructor(importModule, entry)
    {
        this.__importModule = importModule;
        this.__entry = entry;
    }

    toString()
    {
        return this.ModuleName;
    }

    get ModuleName()
    {
        var address = this.__importModule.BaseAddress.add(this.__entry.DllNameRVA);
        return host.memory.readString(address, this.__importModule);
    }

    get IsLoaded()
    {
        return (!(this.ResolvedModule === undefined));
    }

    get ResolvedModule()
    {
        return __findRelatedModule(this.__importModule, this.ModuleName);
    }

    get Functions()
    {
        var thunkTable = this.__importModule.BaseAddress.add(this.__entry.ImportAddressTableRVA);
        return new __ImportFunctions(this.__importModule, thunkTable);
    }
}

// __DelayImportsTable
//
// An abstraction over the delay load import table within an image.
//
class __DelayImportsTable
{
    constructor(importModule, startingEntry)
    {
        this.__importModule = importModule;
        this.__startingEntry = startingEntry;
    }

    getDimensionality()
    {
        return 1;
    }
    
    getValueAt(idx)
    {
        for (var idxEntry of this)
        {
            var entry = idxEntry.value;
            if (entry.ModuleName == idx)
            {
                return entry;
            }
        }
        
        throw new RangeError("Unable to find specified delay import: " + idx);
    }
    
    *[Symbol.iterator]()
    {
        if (this.__startingEntry === undefined)
        {
            return;
        }

        var currentEntry = this.__startingEntry;
        var size = currentEntry.targetSize;
        while (currentEntry.DllNameRVA != 0)
        {
            var entry = new __DelayImportEntry(this.__importModule, currentEntry);
            yield new host.indexedValue(entry, [entry.ModuleName]);
            currentEntry = __createTypedObject(currentEntry.targetLocation.add(size), __keyModule, "_IMAGE_DELAYLOAD_DESCRIPTOR", this.__importModule);
        }
    }
}

//*******************************************************************************
// Export Table Implementation
//

// __OrdinalExport
//
// An abstraction over an ordinal function export within the EAT of an image.
//
class __OrdinalExport
{
    constructor(exportModule, ordinal, codeAddr)
    {
        this.__exportModule = exportModule;
        this.__ordinal = ordinal;
        this.__codeAddr = codeAddr;
    }

    toString()
    {
        return "Function export by ordinal #" + this.OrdinalNumber;
    }

    get OrdinalNumber()
    {
        return this.__ordinal;
    }

    get CodeAddress()
    {
        return this.__codeAddr;
    }

    get Function()
    {
        return host.getModuleSymbol(this.CodeAddress, this.__exportModule);
    }
}

// __NamedExport
//
// An abstraction over a named function export within the EAT of an image.
//
class __NamedExport extends __OrdinalExport
{
    constructor(exportModule, name, ordinal, codeAddr)
    {
        super(exportModule, ordinal, codeAddr);
        this.__name = name;
    }

    toString()
    {
        return "Function export of '" + this.Name + "'";
    }

    get Name()
    {
        return this.__name;
    }
}

// __ExportsTable
//
// An abstraction over the EAT within an image.
//
class __ExportsTable
{
    constructor(exportModule, exportDescriptor)
    {
        this.__exportModule = exportModule;
        this.__exportDescriptor = exportDescriptor;
    }

    *[Symbol.iterator]()
    {
        if (this.__exportDescriptor === undefined)
        {
            return;
        }

        var modBase = this.__exportModule.BaseAddress;

        //
        // nameOrdinals will contains the ordinal index of every named export in the module.
        //
        var nameOrdAddr = modBase.add(this.__exportDescriptor.AddressOfNameOrdinals);
        var nameCount = this.__exportDescriptor.NumberOfNames;
        var nameOrdinals = host.memory.readMemoryValues(nameOrdAddr, nameCount, 2, false, this.__exportModule);

        //
        // funcAddrs will contains the RVA of every export function
        //
        var funcAddrAddr = modBase.add(this.__exportDescriptor.AddressOfFunctions);
        var funcCount = this.__exportDescriptor.NumberOfFunctions;
        var funcAddrs = host.memory.readMemoryValues(funcAddrAddr, funcCount, 4, false, this.__exportModule);

        //
        // Completely invert the ordering: nameOrdinals[i] will give you the ordinal number for the i-th named
        // export function.  We want ordinalNames[o] which will give you the index into the name tables of
        // ordinal o (or zero).
        //
        var ordinalNames = new Array(funcCount);
        for (var i = 0; i < funcCount; ++i)
        {
            ordinalNames[i] = 0xFFFF;
        }
        for (i = 0; i < nameCount; ++i)
        {
            var ordNum = nameOrdinals[i];
            ordinalNames[ordNum] = i;
        }

        //
        // nameStrings will contain the RVA of every string.
        //
        var nameStringsAddr = modBase.add(this.__exportDescriptor.AddressOfNames);
        var nameStrings = host.memory.readMemoryValues(nameStringsAddr, nameCount, 4, false, this.__exportModule);

        var ordinalAddr = modBase.add(this.__exportDescriptor.AddressOfNameOrdinals);
        for (var i = 0; i < funcCount; ++i)
        {
            var codeAddr = modBase.add(funcAddrs[i]);
            var nameIdx = ordinalNames[i];

            //
            // Is this a named or by-ordinal export...?
            //
            if (nameIdx != 0xFFFF)
            {
                var nameAddr = modBase.add(nameStrings[nameIdx]);
                var name = host.memory.readString(nameAddr, this.__exportModule);

                yield new __NamedExport(this.__exportModule, name, i, codeAddr);
            }
            else
            {
                yield new __OrdinalExport(this.__exportModule, i, codeAddr);
            }
        }
    }
}

//*******************************************************************************
// Debug Information:
//

// __RSDSCodeViewData:
//
// Abstraction over an RSDS CodeView entry in the debug directory.
//
class __RSDSCodeViewData
{
    constructor(debugModule, rsdsAddress)
    {
        this.__debugModule = debugModule;
        this.__rsdsAddress = rsdsAddress;
    }

    get Guid()
    {
        return __createTypedObject(this.__rsdsAddress.add(4), __keyModule, "GUID", this.__debugModule);
    }

    get Age()
    {
        return host.memory.readMemoryValues(this.__rsdsAddress.add(20), 1, 4, false, this.__debugModule)[0];
    }
}

//
// __DebugCodeView:
//
// Abstraction over a CodeView debug directory.
//
class __DebugCodeView
{
    constructor(debugModule, debugEntry)
    {
        this.__debugModule = debugModule;
        this.__debugEntry = debugEntry;
        this.__dataAddress = this.__debugModule.BaseAddress.add(this.__debugEntry.AddressOfRawData);
    }

    get Signature()
    {
        var sig = host.memory.readMemoryValues(this.__dataAddress, 1, 4, false, this.__debugModule)[0];
        var sigStr = String.fromCharCode(sig & 0xFF) +
                     String.fromCharCode((sig >> 8) & 0xFF) +
                     String.fromCharCode((sig >> 16) & 0xFF) +
                     String.fromCharCode((sig >> 24) & 0xFF);
        return sigStr;
    }

    get Data()
    {
        var sig = this.Signature;
        if (sig == "RSDS")
        {
            return new __RSDSCodeViewData(this.__debugModule, this.__dataAddress);
        }

        return undefined;
    }
}

//
// __DebugTable:
//
// An interpretive abstraction over the information in the debug directories.
//
class __DebugTable
{
    constructor(debugModule, debugDirectories)
    {
        this.__debugModule = debugModule;
        this.__debugDirectories = debugDirectories;
        this.__debugTypeNumbers =
        {
            unknown : 0,
            coff : 1,
            codeView : 2,
            fpo : 3,
            misc : 4,
            exception : 5,
            fixup : 6,
            omapToSrc : 7,
            omapFromSrc : 8,
            borland : 9,
            reserved10 : 10,
            clsId : 11
        };
    }

    __findEntry(debugType)
    {
        for (var directory of this.__debugDirectories)
        {
            if (directory.Type == debugType)
            {
                return directory;
            }
        }

        return undefined;
    }

    get CodeView()
    {
        var dbgEntry = this.__findEntry(this.__debugTypeNumbers.codeView);
        if (dbgEntry === undefined)
        {
            return undefined;
        }

        return new __DebugCodeView(this.__debugModule, dbgEntry);
    }
}


//*******************************************************************************
// Core Extension
//

//
// _ModuleInformation:
//
// This is our sub-namespace addition to "Module.Contents"
//
class __ModuleInformation
{
    //*************************************************
    // PE Defined Content:
    //

    get Headers()
    {
        var imageInfo = new __ImageInformation(this);
        if (imageInfo.ImageType != "PE")
        {
            return undefined;
        }

        var headers = new __ImageHeaders(this, imageInfo.Information);
        return headers;
    }

    get Directories()
    {
        var imageInfo = new __ImageInformation(this);
        if (imageInfo.ImageType != "PE")
        {
            return undefined;
        }

        var dirs = new __ImageDirectories(this, imageInfo.Information);
        return dirs;
    }
    
    get Resources()
    {
        var imageInfo = new __ImageInformation(this);
        if (imageInfo.ImageType != "PE")
        {
            return undefined;
        }

        var resources = new __ResourceTable(this, imageInfo.Information.ResourceDirectory, imageInfo.Information.ResourceDirectory);
        return resources;
    }

    get Imports()
    {
        var imageInfo = new __ImageInformation(this);
        if (imageInfo.ImageType != "PE")
        {
            return undefined;
        }

        var importTable = new __ImportsTable(this, imageInfo.Information.ImportDescriptor);
        return importTable;
    }

    get DelayImports()
    {
        var imageInfo = new __ImageInformation(this);
        if (imageInfo.ImageType != "PE")
        {
            return undefined;
        }

        var delayImportTable = new __DelayImportsTable(this, imageInfo.Information.DelayImportDescriptor);
        return delayImportTable;
    }

    get Exports()
    {
        var imageInfo = new __ImageInformation(this);
        if (imageInfo.ImageType != "PE")
        {
            return undefined;
        }

        var exportTable = new __ExportsTable(this, imageInfo.Information.ExportDescriptor);
        return exportTable;
    }

    get DebugInfo()
    {
        var imageInfo = new __ImageInformation(this);
        if (imageInfo.ImageType != "PE")
        {
            return undefined;
        }

        var debugTable = new __DebugTable(this, imageInfo.Information.DebugDescriptors);
        return debugTable;
    }

    get Version()
    {
        var imageInfo = new __ImageInformation(this);
        if (imageInfo.ImageType != "PE")
        {
            return undefined;
        }

        var rscTable = this.Contents.Resources;
        if (rscTable === undefined)
        {
            return undefined;
        }
        var versionResource = rscTable.getValueAt(16);  // RT_VERSION
        if (versionResource === undefined)
        {
            return undefined;
        }
        versionResource = versionResource.Children.getValueAt(1);
        if (versionResource === undefined)
        {
            return undefined;
        }
        var byLanguage = versionResource.Children;
        versionResource = undefined;
        for (var first of byLanguage)
        {
            versionResource = first.value;              // Internally an indexedValue
            break;
        }
        if (versionResource === undefined)
        {
            return undefined;
        }
        return new __VersionResourceEntry(this, versionResource.DataAddress, versionResource.Data, versionResource.Data.length, 0, "");
    }

    //*************************************************
    // ELF Defined Content:
    //

    get BuildID()
    {
        var imageInfo = new __ImageInformation(this);
        if (imageInfo.ImageType != "ELF")
        {
            return undefined;
        }

        var phTypes = imageInfo.Information.ProgramHeaderTypes;
        var ntTypes = imageInfo.Information.NoteTypes;

        //
        // Find a PT_NOTE section with a "GNU" note whose type is NT_GNU_BUILD_ID
        //
        for (var header of imageInfo.Information.ProgramHeaders)
        {
            if (header.__programHeader.Type == phTypes.PT_NOTE)
            {
                for (var note of header.Content.Notes)
                {
                    if (note.Name == "GNU" && note.__noteHeader.Type == ntTypes.NT_GNU_BUILD_ID)
                    {
                        return note.Content.BuildID;
                    }
                }
            }
        }

        return undefined;
    }

    get BuildInfo()
    {
        var imageInfo = new __ImageInformation(this);
        if (imageInfo.ImageType != "ELF")
        {
            return undefined;
        }

        var phTypes = imageInfo.Information.ProgramHeaderTypes;
        var ntTypes = imageInfo.Information.NoteTypes;

        //
        // Find a PT_NOTE section with a "FDO" note whose type is NT_FDO_BUILDINFO
        //
        for (var header of imageInfo.Information.ProgramHeaders)
        {
            if (header.__programHeader.Type == phTypes.PT_NOTE)
            {
                for (var note of header.Content.Notes)
                {
                    if (note.Name == "FDO" && note.__noteHeader.Type == ntTypes.NT_FDO_BUILDINFO)
                    {
                        return note.Content.BuildInfo;
                    }
                }
            }
        }

        return undefined;
    }

    get ELFHeader()
    {
        var imageInfo = new __ImageInformation(this);
        if (imageInfo.ImageType != "ELF")
        {
            return undefined;
        }

        return imageInfo.Information.ELFHeader;
    }

    get ProgramHeaders()
    {
        var imageInfo = new __ImageInformation(this);
        if (imageInfo.ImageType != "ELF")
        {
            return undefined;
        }

        return imageInfo.Information.ProgramHeaders;
    }
}

// __VersionFileFlags:
//
// Interprets the flags of a VS_FIXEDFILEINFO
//
class __VersionFileFlags
{
    constructor(versionInfo)
    {
        this.__versionInfo = versionInfo;
        this.__flags = (versionInfo.dwFileFlags & versionInfo.dwFileFlagsMask);
    }

    get IsDebug()
    {
        return (this.__flags & 0x00000001) != 0;
    }

    get IsInfoInferred()
    {
        return (this.__flags & 0x00000010) != 0;
    }

    get IsPatched()
    {
        return (this.__flags & 0x00000004) != 0;
    }

    get IsPreRelease()
    {
        return (this.__flags & 0x00000002) != 0;
    }

    get IsPrivateBuild()
    {
        return (this.__flags & 0x00000008) != 0;
    }

    get IsSpecialBuild()
    {
        return (this.__flags & 0x00000020) != 0;
    }
}

// __FixedVersionInfo:
//
// Visualizer/extension around VS_FIXEDFILEINFO
//
class __FixedVersionInfo
{
    get FileVersion()
    {
        return (this.dwFileVersionMS >> 16).toString() + "." +
               (this.dwFileVersionMS & 0x0000FFFF).toString() + "." +
               (this.dwFileVersionLS >> 16).toString() + "." +
               (this.dwFileVersionLS & 0x0000FFFF);
    }

    get ProductVersion()
    {
        return (this.dwFileVersionMS >> 16).toString() + "." +
               (this.dwFileVersionMS & 0x0000FFFF).toString() + "." +
               (this.dwFileVersionLS >> 16).toString() + "." +
               (this.dwFileVersionLS & 0x0000FFFF);
    }

    get FileFlags()
    {
        return new __VersionFileFlags(this);
    }

    __osString(val)
    {
        switch(val)
        {
            case 0x00010000:
                return "MS-DOS";
            
            case 0x00040000:
            case 0x00040004:
                return "Windows NT";

            case 0x00000001:
                return "16-bit Windows";

            case 0x00000004:
                return "32-bit Windows";

            case 0x00020000:
                return "16-bit OS/2";

            case 0x00030000:
                return "32-bit OS/2";

            case 0x00000002:
                return "16-bit Presentation Manager";

            case 0x00000003:
                return "32-bit Presentation Manager";

            default:
                return "Unknown";
        }
    }

    get OS()
    {
        var osFlags = this.dwFileOS;
        var desc = this.__osString(osFlags);
        if (desc != "Unknown")
        {
            return desc;
        }

        var topNib = (osFlags & 0xFFFF0000);
        var botNib = (osFlags & 0x0000FFFF);
        if (topNib == 0 || botNib == 0)
        {
            return desc;
        }
        else
        {
            return this.__osString(botNib) + " on " + this.__osString(topNib);
        }
    }

    get FileType()
    {
        switch(this.dwFileType)
        {
            case 1:
                return "Application";
            
            case 2:
                return "DLL";
            
            case 3:
                return "Device Driver";
            
            case 4:
                return "Font";
            
            case 7:
                return "Static Link Library";
            
            case 5:
                return "Virtual Device (VXD)";

            default:
                return "Unknown";
        }
    }

    get FileSubType()
    {
        if (this.dwFileType == 3)
        {
            switch(this.dwFileSubtype)
            {
                case 0:
                    return "Communications Driver";
                
                case 4:
                    return "Display Driver";
                
                case 8:
                    return "Installable Driver";
                
                case 2:
                    return "Keyboard Driver";

                case 3:
                    return "Language Driver";
                
                case 5:
                    return "Mouse Driver";
                
                case 6:
                    return "Network Driver";
                
                case 1:
                    return "Printer Driver";
                
                case 9:
                    return "Sound Driver";
                
                case 7:
                    return "System Driver";
                
                case 12:
                    return "Versioned Printer Driver"

                default:
                    return "Unknown Driver";
            }
        }
        else if (this.dwFileType == 4)
        {
            switch(this.dwFileSubtype)
            {
                case 1:
                    return "Raster Font";
                
                case 3:
                    return "TrueType Font";
                
                case 2:
                    return "Vector Font";
                
                default:
                    return "Unknown Font";
            }
        }

        return undefined;
    }
}

//*************************************************
// Private Items:
//
// These properties are only accessible within the bounds of this script.
// They do not marshal into any other context.
//

class __ModulePublic
{
    get ImageType()
    {
        var info = new __ImageInformation(this);
        return info.ImageType;
    }

}

class __ModulePrivate
{
    get __IsPE32()
    {
        var info = new __ImageInformation(this);
        return (info.ImageType == "PE" && info.Information.IsPE32);
    }

    get __ComparisonName()
    {
        var name = this.Name;
        var compName = name.substring(name.lastIndexOf("\\") + 1);
        return compName;
    }
}

function initializeScript()
{
    return [new host.namespacePropertyParent(__ModuleInformation, "Debugger.Models.Module", "Debugger.Models.Module.Contents", "Contents"),
            new host.namedModelParent(__ModulePrivate, "Debugger.Models.Module"),
            new host.namedModelParent(__ModulePublic, "Debugger.Models.Module"),
            new host.optionalRecord(new host.typeSignatureRegistration(__FixedVersionInfo, "VS_FIXEDFILEINFO"))];
     
}

// SIG // Begin signature block
// SIG // MIIl8wYJKoZIhvcNAQcCoIIl5DCCJeACAQExDzANBglg
// SIG // hkgBZQMEAgEFADB3BgorBgEEAYI3AgEEoGkwZzAyBgor
// SIG // BgEEAYI3AgEeMCQCAQEEEBDgyQbOONQRoqMAEEvTUJAC
// SIG // AQACAQACAQACAQACAQAwMTANBglghkgBZQMEAgEFAAQg
// SIG // 63dns8I+YREOMSS0dr9sr8s65n66KzlQxz4rXnW2iUug
// SIG // ggtyMIIE+jCCA+KgAwIBAgITMwAABJAOYRSYEngjcAAA
// SIG // AAAEkDANBgkqhkiG9w0BAQsFADB+MQswCQYDVQQGEwJV
// SIG // UzETMBEGA1UECBMKV2FzaGluZ3RvbjEQMA4GA1UEBxMH
// SIG // UmVkbW9uZDEeMBwGA1UEChMVTWljcm9zb2Z0IENvcnBv
// SIG // cmF0aW9uMSgwJgYDVQQDEx9NaWNyb3NvZnQgQ29kZSBT
// SIG // aWduaW5nIFBDQSAyMDEwMB4XDTIyMDUxMjIwNDcwNVoX
// SIG // DTIzMDUxMTIwNDcwNVowdDELMAkGA1UEBhMCVVMxEzAR
// SIG // BgNVBAgTCldhc2hpbmd0b24xEDAOBgNVBAcTB1JlZG1v
// SIG // bmQxHjAcBgNVBAoTFU1pY3Jvc29mdCBDb3Jwb3JhdGlv
// SIG // bjEeMBwGA1UEAxMVTWljcm9zb2Z0IENvcnBvcmF0aW9u
// SIG // MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA
// SIG // kzKQ8vGa0bmAeZL9lDoj932a4dRNz0Kx3d8KQVWyoUu5
// SIG // KfbowT+sAi2YqzLiGxCaFmlb3NR3nOuLCPbkbHAszb1i
// SIG // e2jFFPegX9Pv5hi4m7kno19cQgSuugPWg0BqduLJjo9Y
// SIG // mYEwPFDxwqATz3OhczReOZSs4WYsPuS5iozdAzsRkyYa
// SIG // AqzEHaCbfjlqF2BuAfDVMNNUUTrWdTP3fu2zSlWOtMCu
// SIG // 3ZiarDD2e+A26rrhQfRNcCKpgIqKxZzuhE+dEsWDjS2l
// SIG // wGI4hGSvJUU+n8/q0gEEv5ihubVFUXA+Rbe+Ng2d9Na2
// SIG // 5n1L2wex6z3H+lfh8gNUl7OuooacTwk3SQIDAQABo4IB
// SIG // eTCCAXUwHwYDVR0lBBgwFgYKKwYBBAGCNz0GAQYIKwYB
// SIG // BQUHAwMwHQYDVR0OBBYEFB6A9Jmd73vEgtVXj6p5ztef
// SIG // OsgFMFAGA1UdEQRJMEekRTBDMSkwJwYDVQQLEyBNaWNy
// SIG // b3NvZnQgT3BlcmF0aW9ucyBQdWVydG8gUmljbzEWMBQG
// SIG // A1UEBRMNMjMwODY1KzQ3MDU2MjAfBgNVHSMEGDAWgBTm
// SIG // /F97uyIAWORyTrX0IXQjMubvrDBWBgNVHR8ETzBNMEug
// SIG // SaBHhkVodHRwOi8vY3JsLm1pY3Jvc29mdC5jb20vcGtp
// SIG // L2NybC9wcm9kdWN0cy9NaWNDb2RTaWdQQ0FfMjAxMC0w
// SIG // Ny0wNi5jcmwwWgYIKwYBBQUHAQEETjBMMEoGCCsGAQUF
// SIG // BzAChj5odHRwOi8vd3d3Lm1pY3Jvc29mdC5jb20vcGtp
// SIG // L2NlcnRzL01pY0NvZFNpZ1BDQV8yMDEwLTA3LTA2LmNy
// SIG // dDAMBgNVHRMBAf8EAjAAMA0GCSqGSIb3DQEBCwUAA4IB
// SIG // AQA+hWJ633NByB9VK8ZlZLCUq2e3X1MdTQe8D7aRgpeX
// SIG // yzMxP8Iapp0wDFvAgACtPXr0DBHleAK3yUhti1xo50f8
// SIG // YgQ+O/a/Tqqp5/TaHzvP5LfJU4Mm4DpSeMQU75/WXdyP
// SIG // wKxRZjD8FdsCelpWgrGa5eCW4lqYQILMpSLO555pSFBA
// SIG // QkrNB1BJtViE0ZzaH+0skTY15S5Zm3m0WEG7VdTCmwcy
// SIG // m1H+MzSZar4wC0XSGzXrd3bDmHJhkws9ORZaka/MlpHa
// SIG // 7LFGKTlfuZ1n1gLBcMx4P48NXB6F5ngq+yJZy1wxiI2W
// SIG // Vh2ymanlnw1a5cKjLH5+xJUk6nMHz47OmIEuMIIGcDCC
// SIG // BFigAwIBAgIKYQxSTAAAAAAAAzANBgkqhkiG9w0BAQsF
// SIG // ADCBiDELMAkGA1UEBhMCVVMxEzARBgNVBAgTCldhc2hp
// SIG // bmd0b24xEDAOBgNVBAcTB1JlZG1vbmQxHjAcBgNVBAoT
// SIG // FU1pY3Jvc29mdCBDb3Jwb3JhdGlvbjEyMDAGA1UEAxMp
// SIG // TWljcm9zb2Z0IFJvb3QgQ2VydGlmaWNhdGUgQXV0aG9y
// SIG // aXR5IDIwMTAwHhcNMTAwNzA2MjA0MDE3WhcNMjUwNzA2
// SIG // MjA1MDE3WjB+MQswCQYDVQQGEwJVUzETMBEGA1UECBMK
// SIG // V2FzaGluZ3RvbjEQMA4GA1UEBxMHUmVkbW9uZDEeMBwG
// SIG // A1UEChMVTWljcm9zb2Z0IENvcnBvcmF0aW9uMSgwJgYD
// SIG // VQQDEx9NaWNyb3NvZnQgQ29kZSBTaWduaW5nIFBDQSAy
// SIG // MDEwMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKC
// SIG // AQEA6Q5kUHlntcTj/QkATJ6UrPdWaOpE2M/FWE+ppXZ8
// SIG // bUW60zmStKQe+fllguQX0o/9RJwI6GWTzixVhL99COMu
// SIG // K6hBKxi3oktuSUxrFQfe0dLCiR5xlM21f0u0rwjYzIjW
// SIG // axeUOpPOJj/s5v40mFfVHV1J9rIqLtWFu1k/+JC0K4N0
// SIG // yiuzO0bj8EZJwRdmVMkcvR3EVWJXcvhnuSUgNN5dpqWV
// SIG // XqsogM3Vsp7lA7Vj07IUyMHIiiYKWX8H7P8O7YASNUwS
// SIG // pr5SW/Wm2uCLC0h31oVH1RC5xuiq7otqLQVcYMa0Kluc
// SIG // IxxfReMaFB5vN8sZM4BqiU2jamZjeJPVMM+VHwIDAQAB
// SIG // o4IB4zCCAd8wEAYJKwYBBAGCNxUBBAMCAQAwHQYDVR0O
// SIG // BBYEFOb8X3u7IgBY5HJOtfQhdCMy5u+sMBkGCSsGAQQB
// SIG // gjcUAgQMHgoAUwB1AGIAQwBBMAsGA1UdDwQEAwIBhjAP
// SIG // BgNVHRMBAf8EBTADAQH/MB8GA1UdIwQYMBaAFNX2VsuP
// SIG // 6KJcYmjRPZSQW9fOmhjEMFYGA1UdHwRPME0wS6BJoEeG
// SIG // RWh0dHA6Ly9jcmwubWljcm9zb2Z0LmNvbS9wa2kvY3Js
// SIG // L3Byb2R1Y3RzL01pY1Jvb0NlckF1dF8yMDEwLTA2LTIz
// SIG // LmNybDBaBggrBgEFBQcBAQROMEwwSgYIKwYBBQUHMAKG
// SIG // Pmh0dHA6Ly93d3cubWljcm9zb2Z0LmNvbS9wa2kvY2Vy
// SIG // dHMvTWljUm9vQ2VyQXV0XzIwMTAtMDYtMjMuY3J0MIGd
// SIG // BgNVHSAEgZUwgZIwgY8GCSsGAQQBgjcuAzCBgTA9Bggr
// SIG // BgEFBQcCARYxaHR0cDovL3d3dy5taWNyb3NvZnQuY29t
// SIG // L1BLSS9kb2NzL0NQUy9kZWZhdWx0Lmh0bTBABggrBgEF
// SIG // BQcCAjA0HjIgHQBMAGUAZwBhAGwAXwBQAG8AbABpAGMA
// SIG // eQBfAFMAdABhAHQAZQBtAGUAbgB0AC4gHTANBgkqhkiG
// SIG // 9w0BAQsFAAOCAgEAGnTvV08pe8QWhXi4UNMi/AmdrIKX
// SIG // +DT/KiyXlRLl5L/Pv5PI4zSp24G43B4AvtI1b6/lf3mV
// SIG // d+UC1PHr2M1OHhthosJaIxrwjKhiUUVnCOM/PB6T+DCF
// SIG // F8g5QKbXDrMhKeWloWmMIpPMdJjnoUdD8lOswA8waX/+
// SIG // 0iUgbW9h098H1dlyACxphnY9UdumOUjJN2FtB91TGcun
// SIG // 1mHCv+KDqw/ga5uV1n0oUbCJSlGkmmzItx9KGg5pqdfc
// SIG // wX7RSXCqtq27ckdjF/qm1qKmhuyoEESbY7ayaYkGx0aG
// SIG // ehg/6MUdIdV7+QIjLcVBy78dTMgW77Gcf/wiS0mKbhXj
// SIG // pn92W9FTeZGFndXS2z1zNfM8rlSyUkdqwKoTldKOEdqZ
// SIG // Z14yjPs3hdHcdYWch8ZaV4XCv90Nj4ybLeu07s8n07Ve
// SIG // afqkFgQBpyRnc89NT7beBVaXevfpUk30dwVPhcbYC/GO
// SIG // 7UIJ0Q124yNWeCImNr7KsYxuqh3khdpHM2KPpMmRM19x
// SIG // HkCvmGXJIuhCISWKHC1g2TeJQYkqFg/XYTyUaGBS79ZH
// SIG // maCAQO4VgXc+nOBTGBpQHTiVmx5mMxMnORd4hzbOTsNf
// SIG // svU9R1O24OXbC2E9KteSLM43Wj5AQjGkHxAIwlacvyRd
// SIG // UQKdannSF9PawZSOB3slcUSrBmrm1MbfI5qWdcUxghnZ
// SIG // MIIZ1QIBATCBlTB+MQswCQYDVQQGEwJVUzETMBEGA1UE
// SIG // CBMKV2FzaGluZ3RvbjEQMA4GA1UEBxMHUmVkbW9uZDEe
// SIG // MBwGA1UEChMVTWljcm9zb2Z0IENvcnBvcmF0aW9uMSgw
// SIG // JgYDVQQDEx9NaWNyb3NvZnQgQ29kZSBTaWduaW5nIFBD
// SIG // QSAyMDEwAhMzAAAEkA5hFJgSeCNwAAAAAASQMA0GCWCG
// SIG // SAFlAwQCAQUAoIIBBDAZBgkqhkiG9w0BCQMxDAYKKwYB
// SIG // BAGCNwIBBDAcBgorBgEEAYI3AgELMQ4wDAYKKwYBBAGC
// SIG // NwIBFTAvBgkqhkiG9w0BCQQxIgQgu+/k2Jzc9YNkY81h
// SIG // i3QakYQD+y5Q0t0SbdX1vH5wuP4wPAYKKwYBBAGCNwoD
// SIG // HDEuDCxzUFk3eFBCN2hUNWc1SEhyWXQ4ckRMU005VnVa
// SIG // UnVXWmFlZjJlMjJSczU0PTBaBgorBgEEAYI3AgEMMUww
// SIG // SqAkgCIATQBpAGMAcgBvAHMAbwBmAHQAIABXAGkAbgBk
// SIG // AG8AdwBzoSKAIGh0dHA6Ly93d3cubWljcm9zb2Z0LmNv
// SIG // bS93aW5kb3dzMA0GCSqGSIb3DQEBAQUABIIBAB7meyMW
// SIG // ecQzwuUxel3NaitBYSkmoueKgaHen+/rJwYK456ciZGZ
// SIG // gKtwhSqDDxhoDRDji+S0ynlCYY5W91AAmm8Cv39ETB+W
// SIG // vBy55GpKBMjIG6Xrd3g5g+qVUYuSEap9XAR5i1+wd3e6
// SIG // 1C+3HVQstYfm9bw9IlQypj3MU4ETkkRkflGoBMm22kwO
// SIG // IKEGkktEPLIW3E+qnCtMWV6rK1RrpamUdBO6RxduMMIn
// SIG // tT1w4FiV0JopcKGmIhBtnxPDlKNJ5c20GXWZjmGZp8GJ
// SIG // OWTtM1BIh3/lBKLiBgIysxz58ali9sBQzkPwbkqzcsxy
// SIG // mRCEmK/y1rslFWeiYxMhVPQhrCKhghcMMIIXCAYKKwYB
// SIG // BAGCNwMDATGCFvgwghb0BgkqhkiG9w0BBwKgghblMIIW
// SIG // 4QIBAzEPMA0GCWCGSAFlAwQCAQUAMIIBVQYLKoZIhvcN
// SIG // AQkQAQSgggFEBIIBQDCCATwCAQEGCisGAQQBhFkKAwEw
// SIG // MTANBglghkgBZQMEAgEFAAQgPkMKfEqz+QylqKaVfScB
// SIG // n2IwyA9uyqERsYwlf6w23+YCBmLbK6FqNBgTMjAyMjA4
// SIG // MDUwNDM0MzMuNzU4WjAEgAIB9KCB1KSB0TCBzjELMAkG
// SIG // A1UEBhMCVVMxEzARBgNVBAgTCldhc2hpbmd0b24xEDAO
// SIG // BgNVBAcTB1JlZG1vbmQxHjAcBgNVBAoTFU1pY3Jvc29m
// SIG // dCBDb3Jwb3JhdGlvbjEpMCcGA1UECxMgTWljcm9zb2Z0
// SIG // IE9wZXJhdGlvbnMgUHVlcnRvIFJpY28xJjAkBgNVBAsT
// SIG // HVRoYWxlcyBUU1MgRVNOOkY4N0EtRTM3NC1EN0I5MSUw
// SIG // IwYDVQQDExxNaWNyb3NvZnQgVGltZS1TdGFtcCBTZXJ2
// SIG // aWNloIIRXzCCBxAwggT4oAMCAQICEzMAAAGuqgtcszSl
// SIG // lRoAAQAAAa4wDQYJKoZIhvcNAQELBQAwfDELMAkGA1UE
// SIG // BhMCVVMxEzARBgNVBAgTCldhc2hpbmd0b24xEDAOBgNV
// SIG // BAcTB1JlZG1vbmQxHjAcBgNVBAoTFU1pY3Jvc29mdCBD
// SIG // b3Jwb3JhdGlvbjEmMCQGA1UEAxMdTWljcm9zb2Z0IFRp
// SIG // bWUtU3RhbXAgUENBIDIwMTAwHhcNMjIwMzAyMTg1MTM3
// SIG // WhcNMjMwNTExMTg1MTM3WjCBzjELMAkGA1UEBhMCVVMx
// SIG // EzARBgNVBAgTCldhc2hpbmd0b24xEDAOBgNVBAcTB1Jl
// SIG // ZG1vbmQxHjAcBgNVBAoTFU1pY3Jvc29mdCBDb3Jwb3Jh
// SIG // dGlvbjEpMCcGA1UECxMgTWljcm9zb2Z0IE9wZXJhdGlv
// SIG // bnMgUHVlcnRvIFJpY28xJjAkBgNVBAsTHVRoYWxlcyBU
// SIG // U1MgRVNOOkY4N0EtRTM3NC1EN0I5MSUwIwYDVQQDExxN
// SIG // aWNyb3NvZnQgVGltZS1TdGFtcCBTZXJ2aWNlMIICIjAN
// SIG // BgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAk4wa8SE1
// SIG // DAsdpy3Oc+ljwmDmojxCyCnSaGXYbbO1U+ieriCw4x7m
// SIG // 72nl/Xs8gzUpeNRoo2Xd2Odyrb0uKqaqdoo5GCA8c0ST
// SIG // tD61qXkjJz5LyT9HfWAIa3iq9BWoEtA2K/E66RR9qkbj
// SIG // UtN0sd4zi7AieT5CsZAfYrjCM22JSmKsXY90JxuRfIAs
// SIG // SnJPZGvDMmbNyZt0KxxjQ3dEfGsx5ZDeTuw23jU0Fk5P
// SIG // 7ikKaTDxSSAqJIlczMqzfwzFSrH86VLzR0sNMd35l6LV
// SIG // LX+psK1MbM2bRuPqp+SVQzckUAXUktfDC+qBlF0NBTrb
// SIG // bjC0afBqVNo4jRHR5f5ytw+lcYHbsQiBhT7SWjZofv1I
// SIG // 2uw9YRx0EgJ3TJ+EVTaeJUl6kbORd60m9sXFbeI3uxyM
// SIG // t/D9LpRcXvC0TN041dWIjk/ZQzvv0/oQhn6DzUTYxZfx
// SIG // eMtXK8iy/PJyQngUWL6HXI8T6/NyQ/HMc6yItpp+5yzI
// SIG // yMBoAzxbBr7TYG6MQ7KV8tLKTSK/0i9Ij1mQlb+Au9Dj
// SIG // ZTT5TTflmFSEKpsoRYQwivbJratimtQwQpxd/hH3stU8
// SIG // F+wmduQ1S5ulQDgrWLuKNDWmRSW35hD/fia0TLt5KKBW
// SIG // lXOep+s1V6sK8cbkjB94VWE81sDArqUERDb2cxiNFePh
// SIG // AvK+YpGao4kz/DUCAwEAAaOCATYwggEyMB0GA1UdDgQW
// SIG // BBTTMG/fvyhgisGprXT+/O1kOmFR7jAfBgNVHSMEGDAW
// SIG // gBSfpxVdAF5iXYP05dJlpxtTNRnpcjBfBgNVHR8EWDBW
// SIG // MFSgUqBQhk5odHRwOi8vd3d3Lm1pY3Jvc29mdC5jb20v
// SIG // cGtpb3BzL2NybC9NaWNyb3NvZnQlMjBUaW1lLVN0YW1w
// SIG // JTIwUENBJTIwMjAxMCgxKS5jcmwwbAYIKwYBBQUHAQEE
// SIG // YDBeMFwGCCsGAQUFBzAChlBodHRwOi8vd3d3Lm1pY3Jv
// SIG // c29mdC5jb20vcGtpb3BzL2NlcnRzL01pY3Jvc29mdCUy
// SIG // MFRpbWUtU3RhbXAlMjBQQ0ElMjAyMDEwKDEpLmNydDAM
// SIG // BgNVHRMBAf8EAjAAMBMGA1UdJQQMMAoGCCsGAQUFBwMI
// SIG // MA0GCSqGSIb3DQEBCwUAA4ICAQBdv5Hw/VUARA48rTMD
// SIG // EAMhc/hwlCZqu2NUUswSQtiHf08W1Vu3zhG/RDUZJNia
// SIG // E/x/846+eYLl6PDc1zVVGLvitYZQhO/Xxaqvx4G8BJ3h
// SIG // 4MDEVsDySc46b9nJKQwMNh1vrvfxpDTK+p/sBZyGA+e0
// SIG // Jz+eE1qlImaPNSR7sS+MHx6LQGdjTGX4BBxLEkb9Weyb
// SIG // 0jA56vwTWaJUth8+f18gN1pq/Vur2L6Cdl/WFLtqkanF
// SIG // uK0ImvUoYPiMjIAGTEeF6g86GG1CbW7OcTtuUrEfylTt
// SIG // bYD56qCCw2QzdUHSevNFkGqbhKYFI2E4/PLeh86YtxEr
// SIG // 9qWg4Cvqd6GLyLmWGZODUuQ4DEKEvAe+W6IJj0r7a8im
// SIG // 3jyKgr+H63PlGBV1v5LzHCfvbyU3wo+SQHZFrmKJyu+2
// SIG // ADnnBJR2HoUXFfF5L5uyAFrKftnJp9OkMzsFA4FjBqh2
// SIG // y5V/leAavIbHziThHnyY/AHdDT0JEAazfk063pOs9epz
// SIG // KU27pnPzFNANxomnnikrI6hbmIgMWOkud5dMSO1YIUKA
// SIG // CjjNun0I0hOn3so+dzeBlVoy8SlTxKntVnA31yRHZYMr
// SIG // I6MOCEhx+4UlMs52Q64wsaxY92djqJ21ZzZtQNBrZBvO
// SIG // Y1JnIW2ESmvBDYaaBoZsYq5hVWpSP9i3bUcPQ8F4Mjkx
// SIG // qXxJzDCCB3EwggVZoAMCAQICEzMAAAAVxedrngKbSZkA
// SIG // AAAAABUwDQYJKoZIhvcNAQELBQAwgYgxCzAJBgNVBAYT
// SIG // AlVTMRMwEQYDVQQIEwpXYXNoaW5ndG9uMRAwDgYDVQQH
// SIG // EwdSZWRtb25kMR4wHAYDVQQKExVNaWNyb3NvZnQgQ29y
// SIG // cG9yYXRpb24xMjAwBgNVBAMTKU1pY3Jvc29mdCBSb290
// SIG // IENlcnRpZmljYXRlIEF1dGhvcml0eSAyMDEwMB4XDTIx
// SIG // MDkzMDE4MjIyNVoXDTMwMDkzMDE4MzIyNVowfDELMAkG
// SIG // A1UEBhMCVVMxEzARBgNVBAgTCldhc2hpbmd0b24xEDAO
// SIG // BgNVBAcTB1JlZG1vbmQxHjAcBgNVBAoTFU1pY3Jvc29m
// SIG // dCBDb3Jwb3JhdGlvbjEmMCQGA1UEAxMdTWljcm9zb2Z0
// SIG // IFRpbWUtU3RhbXAgUENBIDIwMTAwggIiMA0GCSqGSIb3
// SIG // DQEBAQUAA4ICDwAwggIKAoICAQDk4aZM57RyIQt5osvX
// SIG // JHm9DtWC0/3unAcH0qlsTnXIyjVX9gF/bErg4r25Phdg
// SIG // M/9cT8dm95VTcVrifkpa/rg2Z4VGIwy1jRPPdzLAEBjo
// SIG // YH1qUoNEt6aORmsHFPPFdvWGUNzBRMhxXFExN6AKOG6N
// SIG // 7dcP2CZTfDlhAnrEqv1yaa8dq6z2Nr41JmTamDu6Gnsz
// SIG // rYBbfowQHJ1S/rboYiXcag/PXfT+jlPP1uyFVk3v3byN
// SIG // pOORj7I5LFGc6XBpDco2LXCOMcg1KL3jtIckw+DJj361
// SIG // VI/c+gVVmG1oO5pGve2krnopN6zL64NF50ZuyjLVwIYw
// SIG // XE8s4mKyzbnijYjklqwBSru+cakXW2dg3viSkR4dPf0g
// SIG // z3N9QZpGdc3EXzTdEonW/aUgfX782Z5F37ZyL9t9X4C6
// SIG // 26p+Nuw2TPYrbqgSUei/BQOj0XOmTTd0lBw0gg/wEPK3
// SIG // Rxjtp+iZfD9M269ewvPV2HM9Q07BMzlMjgK8QmguEOqE
// SIG // UUbi0b1qGFphAXPKZ6Je1yh2AuIzGHLXpyDwwvoSCtdj
// SIG // bwzJNmSLW6CmgyFdXzB0kZSU2LlQ+QuJYfM2BjUYhEfb
// SIG // 3BvR/bLUHMVr9lxSUV0S2yW6r1AFemzFER1y7435UsSF
// SIG // F5PAPBXbGjfHCBUYP3irRbb1Hode2o+eFnJpxq57t7c+
// SIG // auIurQIDAQABo4IB3TCCAdkwEgYJKwYBBAGCNxUBBAUC
// SIG // AwEAATAjBgkrBgEEAYI3FQIEFgQUKqdS/mTEmr6CkTxG
// SIG // NSnPEP8vBO4wHQYDVR0OBBYEFJ+nFV0AXmJdg/Tl0mWn
// SIG // G1M1GelyMFwGA1UdIARVMFMwUQYMKwYBBAGCN0yDfQEB
// SIG // MEEwPwYIKwYBBQUHAgEWM2h0dHA6Ly93d3cubWljcm9z
// SIG // b2Z0LmNvbS9wa2lvcHMvRG9jcy9SZXBvc2l0b3J5Lmh0
// SIG // bTATBgNVHSUEDDAKBggrBgEFBQcDCDAZBgkrBgEEAYI3
// SIG // FAIEDB4KAFMAdQBiAEMAQTALBgNVHQ8EBAMCAYYwDwYD
// SIG // VR0TAQH/BAUwAwEB/zAfBgNVHSMEGDAWgBTV9lbLj+ii
// SIG // XGJo0T2UkFvXzpoYxDBWBgNVHR8ETzBNMEugSaBHhkVo
// SIG // dHRwOi8vY3JsLm1pY3Jvc29mdC5jb20vcGtpL2NybC9w
// SIG // cm9kdWN0cy9NaWNSb29DZXJBdXRfMjAxMC0wNi0yMy5j
// SIG // cmwwWgYIKwYBBQUHAQEETjBMMEoGCCsGAQUFBzAChj5o
// SIG // dHRwOi8vd3d3Lm1pY3Jvc29mdC5jb20vcGtpL2NlcnRz
// SIG // L01pY1Jvb0NlckF1dF8yMDEwLTA2LTIzLmNydDANBgkq
// SIG // hkiG9w0BAQsFAAOCAgEAnVV9/Cqt4SwfZwExJFvhnnJL
// SIG // /Klv6lwUtj5OR2R4sQaTlz0xM7U518JxNj/aZGx80HU5
// SIG // bbsPMeTCj/ts0aGUGCLu6WZnOlNN3Zi6th542DYunKmC
// SIG // VgADsAW+iehp4LoJ7nvfam++Kctu2D9IdQHZGN5tggz1
// SIG // bSNU5HhTdSRXud2f8449xvNo32X2pFaq95W2KFUn0CS9
// SIG // QKC/GbYSEhFdPSfgQJY4rPf5KYnDvBewVIVCs/wMnosZ
// SIG // iefwC2qBwoEZQhlSdYo2wh3DYXMuLGt7bj8sCXgU6ZGy
// SIG // qVvfSaN0DLzskYDSPeZKPmY7T7uG+jIa2Zb0j/aRAfbO
// SIG // xnT99kxybxCrdTDFNLB62FD+CljdQDzHVG2dY3RILLFO
// SIG // Ry3BFARxv2T5JL5zbcqOCb2zAVdJVGTZc9d/HltEAY5a
// SIG // GZFrDZ+kKNxnGSgkujhLmm77IVRrakURR6nxt67I6Ile
// SIG // T53S0Ex2tVdUCbFpAUR+fKFhbHP+CrvsQWY9af3LwUFJ
// SIG // fn6Tvsv4O+S3Fb+0zj6lMVGEvL8CwYKiexcdFYmNcP7n
// SIG // tdAoGokLjzbaukz5m/8K6TT4JDVnK+ANuOaMmdbhIurw
// SIG // J0I9JZTmdHRbatGePu1+oDEzfbzL6Xu/OHBE0ZDxyKs6
// SIG // ijoIYn/ZcGNTTY3ugm2lBRDBcQZqELQdVTNYs6FwZvKh
// SIG // ggLSMIICOwIBATCB/KGB1KSB0TCBzjELMAkGA1UEBhMC
// SIG // VVMxEzARBgNVBAgTCldhc2hpbmd0b24xEDAOBgNVBAcT
// SIG // B1JlZG1vbmQxHjAcBgNVBAoTFU1pY3Jvc29mdCBDb3Jw
// SIG // b3JhdGlvbjEpMCcGA1UECxMgTWljcm9zb2Z0IE9wZXJh
// SIG // dGlvbnMgUHVlcnRvIFJpY28xJjAkBgNVBAsTHVRoYWxl
// SIG // cyBUU1MgRVNOOkY4N0EtRTM3NC1EN0I5MSUwIwYDVQQD
// SIG // ExxNaWNyb3NvZnQgVGltZS1TdGFtcCBTZXJ2aWNloiMK
// SIG // AQEwBwYFKw4DAhoDFQC8mrCT/GfJyBXkZ3LlvgjAT9Na
// SIG // 46CBgzCBgKR+MHwxCzAJBgNVBAYTAlVTMRMwEQYDVQQI
// SIG // EwpXYXNoaW5ndG9uMRAwDgYDVQQHEwdSZWRtb25kMR4w
// SIG // HAYDVQQKExVNaWNyb3NvZnQgQ29ycG9yYXRpb24xJjAk
// SIG // BgNVBAMTHU1pY3Jvc29mdCBUaW1lLVN0YW1wIFBDQSAy
// SIG // MDEwMA0GCSqGSIb3DQEBBQUAAgUA5pbM7jAiGA8yMDIy
// SIG // MDgwNTAyNTU0MloYDzIwMjIwODA2MDI1NTQyWjB3MD0G
// SIG // CisGAQQBhFkKBAExLzAtMAoCBQDmlszuAgEAMAoCAQAC
// SIG // AgGDAgH/MAcCAQACAhENMAoCBQDmmB5uAgEAMDYGCisG
// SIG // AQQBhFkKBAIxKDAmMAwGCisGAQQBhFkKAwKgCjAIAgEA
// SIG // AgMHoSChCjAIAgEAAgMBhqAwDQYJKoZIhvcNAQEFBQAD
// SIG // gYEAmdyh+8lv7hV/DqhR3KJQIyRHONtDhFyVMPBZURQB
// SIG // RtlBOcu4ljBMcQFc5GOKaBe+0/nyLOzdKeuHP3ihjprv
// SIG // S55nM7n9JLxAzWFupEfqNolrhJZlEzfhQ5u84f+JAThF
// SIG // LSQ7n+83UUPsx9lnp6n+NO1cJi/yKPfxcOH7+s318pIx
// SIG // ggQNMIIECQIBATCBkzB8MQswCQYDVQQGEwJVUzETMBEG
// SIG // A1UECBMKV2FzaGluZ3RvbjEQMA4GA1UEBxMHUmVkbW9u
// SIG // ZDEeMBwGA1UEChMVTWljcm9zb2Z0IENvcnBvcmF0aW9u
// SIG // MSYwJAYDVQQDEx1NaWNyb3NvZnQgVGltZS1TdGFtcCBQ
// SIG // Q0EgMjAxMAITMwAAAa6qC1yzNKWVGgABAAABrjANBglg
// SIG // hkgBZQMEAgEFAKCCAUowGgYJKoZIhvcNAQkDMQ0GCyqG
// SIG // SIb3DQEJEAEEMC8GCSqGSIb3DQEJBDEiBCBumiDF9f1W
// SIG // ihygrcEwwmRhxNO+YPhb0eUJe80iWpO9TDCB+gYLKoZI
// SIG // hvcNAQkQAi8xgeowgecwgeQwgb0EIEkoHT64jMNaoe6f
// SIG // T2apNTy46Dq17DTK7W5DSJT8Um9oMIGYMIGApH4wfDEL
// SIG // MAkGA1UEBhMCVVMxEzARBgNVBAgTCldhc2hpbmd0b24x
// SIG // EDAOBgNVBAcTB1JlZG1vbmQxHjAcBgNVBAoTFU1pY3Jv
// SIG // c29mdCBDb3Jwb3JhdGlvbjEmMCQGA1UEAxMdTWljcm9z
// SIG // b2Z0IFRpbWUtU3RhbXAgUENBIDIwMTACEzMAAAGuqgtc
// SIG // szSllRoAAQAAAa4wIgQg2cSgcvdkiMLf4cPN/aMQp59C
// SIG // 0fPib6TnIEZBfU92xk8wDQYJKoZIhvcNAQELBQAEggIA
// SIG // R71jZ9XgLkf5WI6hltC9JDJMmWCudZ1iww2Wl6Ld46O1
// SIG // ZDRHl1I1XXF1tbKO+UqXWeYS1Os+JTNYMngVNZ4U13X1
// SIG // TfCzWKzI3ok6ILrYog/ou1Ggi1OaENTMRwp1+CXFOXbR
// SIG // qFddYD0xQxQ121SYSbxBDuZ2xoBpvdWhV9x/0nl62I/0
// SIG // bj4ravZBht5o6f0SB0Shi9rwSm/UVs+1P1pViclfp3MB
// SIG // 8V0ZSm6H0v1gcrj3emekcR8moFUlymnvVwXWsvTMH0cz
// SIG // lHWH3JRKPJXu5K3b53VT9b3PFehD3Ka8lS+RA/REma4p
// SIG // DeO1Zp98mgWdPddOFFitlywMSc3AreHFyK56gFpz84pj
// SIG // 5z1RRLJkRlNjQkG3/ABlZLku8M9Qbgus2hBmjQ7QxdYI
// SIG // z/E5UPKXruKHkNwoBoHOyGg2VsuCh/8tR7r/Fg6glE48
// SIG // oweDSSnF03y8TJmIR1FZmUkpaaQdT7TJ0CYxfmfPXTf+
// SIG // MjcE94EZeA4VjFTkS5vC2d7yiweRzc7FbnjWya5VmeyQ
// SIG // oYaRgoyqqLrBGGBcOt/31tXg01uN1KCxyJSUNAk6ExbI
// SIG // qapiKc2R9RhbE41RZpEg0d7RXl8SCtoL6IMijNhyGQG/
// SIG // 1Gz/TNSCjKWiwqZ4GvjNJSpZeH5JRpXmG4kuUx0K384P
// SIG // ht7RnAaMv99MtPFkFxl943c=
// SIG // End signature block
