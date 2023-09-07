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
            NT_LPWSINFO: 17
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
            return new __ELFNoteContent(this);
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
// SIG // MIIh2wYJKoZIhvcNAQcCoIIhzDCCIcgCAQExDzANBglg
// SIG // hkgBZQMEAgEFADB3BgorBgEEAYI3AgEEoGkwZzAyBgor
// SIG // BgEEAYI3AgEeMCQCAQEEEBDgyQbOONQRoqMAEEvTUJAC
// SIG // AQACAQACAQACAQACAQAwMTANBglghkgBZQMEAgEFAAQg
// SIG // PNTVon1j8thz3xmVSzlNvCw+n7U+dsvdOPR4o3v2Dxag
// SIG // gguBMIIFCTCCA/GgAwIBAgITMwAAA4Pq27vZbyG4+gAA
// SIG // AAADgzANBgkqhkiG9w0BAQsFADB+MQswCQYDVQQGEwJV
// SIG // UzETMBEGA1UECBMKV2FzaGluZ3RvbjEQMA4GA1UEBxMH
// SIG // UmVkbW9uZDEeMBwGA1UEChMVTWljcm9zb2Z0IENvcnBv
// SIG // cmF0aW9uMSgwJgYDVQQDEx9NaWNyb3NvZnQgQ29kZSBT
// SIG // aWduaW5nIFBDQSAyMDEwMB4XDTIwMDkyNDE5MTAzNVoX
// SIG // DTIxMDkyMzE5MTAzNVowfzELMAkGA1UEBhMCVVMxEzAR
// SIG // BgNVBAgTCldhc2hpbmd0b24xEDAOBgNVBAcTB1JlZG1v
// SIG // bmQxHjAcBgNVBAoTFU1pY3Jvc29mdCBDb3Jwb3JhdGlv
// SIG // bjEpMCcGA1UEAxMgTWljcm9zb2Z0IFdpbmRvd3MgS2l0
// SIG // cyBQdWJsaXNoZXIwggEiMA0GCSqGSIb3DQEBAQUAA4IB
// SIG // DwAwggEKAoIBAQCnt52Kmu4L6Ba3yRL/0obWfohcrE/X
// SIG // yxFhdCkbiMbB55kRKDvAu37z1cSAonm45shuXzH5EWIB
// SIG // w06wzKPt0RHFt9yp5JNQOgglKttQ+JNOjhCA5Rr6rKwP
// SIG // PjPstu/8doUJ5uvSZR6tqYee/f5krppjMclmkNC2Dgrw
// SIG // 9yOmYt5BUvCUNAto/EaX5UhodlIfdF9aNZZ5mY0+dvjq
// SIG // P99wtsRirRmkPLKyAclZHV10w0qhAcANGPLjNurpjiBI
// SIG // T5oQWOYOAGX8A91Bovzwf1Qh3HsYSPE6YdIprFLodldJ
// SIG // TawSwJSBWRWXmFjtQg0zNP8YGOKFt0CdwappX4L/eIA4
// SIG // ELIRAgMBAAGjggF9MIIBeTAfBgNVHSUEGDAWBgorBgEE
// SIG // AYI3CgMUBggrBgEFBQcDAzAdBgNVHQ4EFgQUeEXr3DrQ
// SIG // 1r71bRhURXpiKYw85w4wVAYDVR0RBE0wS6RJMEcxLTAr
// SIG // BgNVBAsTJE1pY3Jvc29mdCBJcmVsYW5kIE9wZXJhdGlv
// SIG // bnMgTGltaXRlZDEWMBQGA1UEBRMNMjI5OTAzKzQ2MTIx
// SIG // NDAfBgNVHSMEGDAWgBTm/F97uyIAWORyTrX0IXQjMubv
// SIG // rDBWBgNVHR8ETzBNMEugSaBHhkVodHRwOi8vY3JsLm1p
// SIG // Y3Jvc29mdC5jb20vcGtpL2NybC9wcm9kdWN0cy9NaWND
// SIG // b2RTaWdQQ0FfMjAxMC0wNy0wNi5jcmwwWgYIKwYBBQUH
// SIG // AQEETjBMMEoGCCsGAQUFBzAChj5odHRwOi8vd3d3Lm1p
// SIG // Y3Jvc29mdC5jb20vcGtpL2NlcnRzL01pY0NvZFNpZ1BD
// SIG // QV8yMDEwLTA3LTA2LmNydDAMBgNVHRMBAf8EAjAAMA0G
// SIG // CSqGSIb3DQEBCwUAA4IBAQC7A8TJmY/vbWihRJK+FlTv
// SIG // R+kHW/TbpVlSyuTw8/V+YfbD7JmqYLs0xLqSIq5qvDst
// SIG // 24lUOfVI2bBLBgWE/jsMZQ9QBgmEiVuw2TwgGbdyXHN0
// SIG // rggJC8zzZMfqEtyho+caqFMzLIELfAEVWd3kIAWYVNzm
// SIG // rD1rpLs3HwnIivyR9mwZyBZfT0k4lrLUUgfERjIeQcAA
// SIG // IzoKS11WsIE1s6UuCKSw2Fs3XnybcnugQrOWDGk4CYUA
// SIG // B9b1A6ShHUNybwA88859RdDY0am0aaytgAtlGEyCn/Cf
// SIG // MmpAjaLtCTYQNw7lHu63HHiMIuJ9pVvUc/0E1rczi3oU
// SIG // h+pM7d88sDdZMIIGcDCCBFigAwIBAgIKYQxSTAAAAAAA
// SIG // AzANBgkqhkiG9w0BAQsFADCBiDELMAkGA1UEBhMCVVMx
// SIG // EzARBgNVBAgTCldhc2hpbmd0b24xEDAOBgNVBAcTB1Jl
// SIG // ZG1vbmQxHjAcBgNVBAoTFU1pY3Jvc29mdCBDb3Jwb3Jh
// SIG // dGlvbjEyMDAGA1UEAxMpTWljcm9zb2Z0IFJvb3QgQ2Vy
// SIG // dGlmaWNhdGUgQXV0aG9yaXR5IDIwMTAwHhcNMTAwNzA2
// SIG // MjA0MDE3WhcNMjUwNzA2MjA1MDE3WjB+MQswCQYDVQQG
// SIG // EwJVUzETMBEGA1UECBMKV2FzaGluZ3RvbjEQMA4GA1UE
// SIG // BxMHUmVkbW9uZDEeMBwGA1UEChMVTWljcm9zb2Z0IENv
// SIG // cnBvcmF0aW9uMSgwJgYDVQQDEx9NaWNyb3NvZnQgQ29k
// SIG // ZSBTaWduaW5nIFBDQSAyMDEwMIIBIjANBgkqhkiG9w0B
// SIG // AQEFAAOCAQ8AMIIBCgKCAQEA6Q5kUHlntcTj/QkATJ6U
// SIG // rPdWaOpE2M/FWE+ppXZ8bUW60zmStKQe+fllguQX0o/9
// SIG // RJwI6GWTzixVhL99COMuK6hBKxi3oktuSUxrFQfe0dLC
// SIG // iR5xlM21f0u0rwjYzIjWaxeUOpPOJj/s5v40mFfVHV1J
// SIG // 9rIqLtWFu1k/+JC0K4N0yiuzO0bj8EZJwRdmVMkcvR3E
// SIG // VWJXcvhnuSUgNN5dpqWVXqsogM3Vsp7lA7Vj07IUyMHI
// SIG // iiYKWX8H7P8O7YASNUwSpr5SW/Wm2uCLC0h31oVH1RC5
// SIG // xuiq7otqLQVcYMa0KlucIxxfReMaFB5vN8sZM4BqiU2j
// SIG // amZjeJPVMM+VHwIDAQABo4IB4zCCAd8wEAYJKwYBBAGC
// SIG // NxUBBAMCAQAwHQYDVR0OBBYEFOb8X3u7IgBY5HJOtfQh
// SIG // dCMy5u+sMBkGCSsGAQQBgjcUAgQMHgoAUwB1AGIAQwBB
// SIG // MAsGA1UdDwQEAwIBhjAPBgNVHRMBAf8EBTADAQH/MB8G
// SIG // A1UdIwQYMBaAFNX2VsuP6KJcYmjRPZSQW9fOmhjEMFYG
// SIG // A1UdHwRPME0wS6BJoEeGRWh0dHA6Ly9jcmwubWljcm9z
// SIG // b2Z0LmNvbS9wa2kvY3JsL3Byb2R1Y3RzL01pY1Jvb0Nl
// SIG // ckF1dF8yMDEwLTA2LTIzLmNybDBaBggrBgEFBQcBAQRO
// SIG // MEwwSgYIKwYBBQUHMAKGPmh0dHA6Ly93d3cubWljcm9z
// SIG // b2Z0LmNvbS9wa2kvY2VydHMvTWljUm9vQ2VyQXV0XzIw
// SIG // MTAtMDYtMjMuY3J0MIGdBgNVHSAEgZUwgZIwgY8GCSsG
// SIG // AQQBgjcuAzCBgTA9BggrBgEFBQcCARYxaHR0cDovL3d3
// SIG // dy5taWNyb3NvZnQuY29tL1BLSS9kb2NzL0NQUy9kZWZh
// SIG // dWx0Lmh0bTBABggrBgEFBQcCAjA0HjIgHQBMAGUAZwBh
// SIG // AGwAXwBQAG8AbABpAGMAeQBfAFMAdABhAHQAZQBtAGUA
// SIG // bgB0AC4gHTANBgkqhkiG9w0BAQsFAAOCAgEAGnTvV08p
// SIG // e8QWhXi4UNMi/AmdrIKX+DT/KiyXlRLl5L/Pv5PI4zSp
// SIG // 24G43B4AvtI1b6/lf3mVd+UC1PHr2M1OHhthosJaIxrw
// SIG // jKhiUUVnCOM/PB6T+DCFF8g5QKbXDrMhKeWloWmMIpPM
// SIG // dJjnoUdD8lOswA8waX/+0iUgbW9h098H1dlyACxphnY9
// SIG // UdumOUjJN2FtB91TGcun1mHCv+KDqw/ga5uV1n0oUbCJ
// SIG // SlGkmmzItx9KGg5pqdfcwX7RSXCqtq27ckdjF/qm1qKm
// SIG // huyoEESbY7ayaYkGx0aGehg/6MUdIdV7+QIjLcVBy78d
// SIG // TMgW77Gcf/wiS0mKbhXjpn92W9FTeZGFndXS2z1zNfM8
// SIG // rlSyUkdqwKoTldKOEdqZZ14yjPs3hdHcdYWch8ZaV4XC
// SIG // v90Nj4ybLeu07s8n07VeafqkFgQBpyRnc89NT7beBVaX
// SIG // evfpUk30dwVPhcbYC/GO7UIJ0Q124yNWeCImNr7KsYxu
// SIG // qh3khdpHM2KPpMmRM19xHkCvmGXJIuhCISWKHC1g2TeJ
// SIG // QYkqFg/XYTyUaGBS79ZHmaCAQO4VgXc+nOBTGBpQHTiV
// SIG // mx5mMxMnORd4hzbOTsNfsvU9R1O24OXbC2E9KteSLM43
// SIG // Wj5AQjGkHxAIwlacvyRdUQKdannSF9PawZSOB3slcUSr
// SIG // Bmrm1MbfI5qWdcUxghWyMIIVrgIBATCBlTB+MQswCQYD
// SIG // VQQGEwJVUzETMBEGA1UECBMKV2FzaGluZ3RvbjEQMA4G
// SIG // A1UEBxMHUmVkbW9uZDEeMBwGA1UEChMVTWljcm9zb2Z0
// SIG // IENvcnBvcmF0aW9uMSgwJgYDVQQDEx9NaWNyb3NvZnQg
// SIG // Q29kZSBTaWduaW5nIFBDQSAyMDEwAhMzAAADg+rbu9lv
// SIG // Ibj6AAAAAAODMA0GCWCGSAFlAwQCAQUAoIIBBDAZBgkq
// SIG // hkiG9w0BCQMxDAYKKwYBBAGCNwIBBDAcBgorBgEEAYI3
// SIG // AgELMQ4wDAYKKwYBBAGCNwIBFTAvBgkqhkiG9w0BCQQx
// SIG // IgQgEiWlD7XKqIF4XUDjKgt+7wBqdo5hPD6TyWUs314a
// SIG // W/MwPAYKKwYBBAGCNwoDHDEuDCxDczNGbktrdkVWYkZj
// SIG // MGlKTTg4Znh5cEJIc2EyeENTSWtjM3c5SlAxVzdZPTBa
// SIG // BgorBgEEAYI3AgEMMUwwSqAkgCIATQBpAGMAcgBvAHMA
// SIG // bwBmAHQAIABXAGkAbgBkAG8AdwBzoSKAIGh0dHA6Ly93
// SIG // d3cubWljcm9zb2Z0LmNvbS93aW5kb3dzMA0GCSqGSIb3
// SIG // DQEBAQUABIIBAE4WeSawv1Ez0j00+vqjxe9KB/n3x9JR
// SIG // 8Ubxuvh7z9LlBfgBbcOMaRUMSLSVgIIKtn+eRKpaQcxR
// SIG // YIMmCMPnjLmY7Rro+5Ixs/tDyr/eavI95n4MfzGsGe+F
// SIG // Aiz+KD5gChsdk7bcZdA3PWXk0ecYIExSHvnE+sn1UFpp
// SIG // FnfW955tLyCpq3x2UKV++Tfxbn2wG5ECHorvsZ10wx1a
// SIG // 5VRoX9n10GI3vM0qxgIbEjChK6K1Ho+XsnDMuatTldv+
// SIG // /9i1B0PKdP24kYdB390s4B7KMQwY0ilLTboyg7M0x6qq
// SIG // bgPDFz1tCiS0IyHDBaibwUgvjZLsjxuEkhctW25HkHuG
// SIG // 5juhghLlMIIS4QYKKwYBBAGCNwMDATGCEtEwghLNBgkq
// SIG // hkiG9w0BBwKgghK+MIISugIBAzEPMA0GCWCGSAFlAwQC
// SIG // AQUAMIIBUQYLKoZIhvcNAQkQAQSgggFABIIBPDCCATgC
// SIG // AQEGCisGAQQBhFkKAwEwMTANBglghkgBZQMEAgEFAAQg
// SIG // XROFimEVWlOmfLOQbmBKudkkq50yl78Y9x4Q0/lxdN0C
// SIG // BmCJ1h39aBgTMjAyMTA1MDgwMDQzMzMuMjM5WjAEgAIB
// SIG // 9KCB0KSBzTCByjELMAkGA1UEBhMCVVMxEzARBgNVBAgT
// SIG // Cldhc2hpbmd0b24xEDAOBgNVBAcTB1JlZG1vbmQxHjAc
// SIG // BgNVBAoTFU1pY3Jvc29mdCBDb3Jwb3JhdGlvbjElMCMG
// SIG // A1UECxMcTWljcm9zb2Z0IEFtZXJpY2EgT3BlcmF0aW9u
// SIG // czEmMCQGA1UECxMdVGhhbGVzIFRTUyBFU046M0JCRC1F
// SIG // MzM4LUU5QTExJTAjBgNVBAMTHE1pY3Jvc29mdCBUaW1l
// SIG // LVN0YW1wIFNlcnZpY2Wggg48MIIE8TCCA9mgAwIBAgIT
// SIG // MwAAAU9kLnX2egNagwAAAAABTzANBgkqhkiG9w0BAQsF
// SIG // ADB8MQswCQYDVQQGEwJVUzETMBEGA1UECBMKV2FzaGlu
// SIG // Z3RvbjEQMA4GA1UEBxMHUmVkbW9uZDEeMBwGA1UEChMV
// SIG // TWljcm9zb2Z0IENvcnBvcmF0aW9uMSYwJAYDVQQDEx1N
// SIG // aWNyb3NvZnQgVGltZS1TdGFtcCBQQ0EgMjAxMDAeFw0y
// SIG // MDExMTIxODI2MDJaFw0yMjAyMTExODI2MDJaMIHKMQsw
// SIG // CQYDVQQGEwJVUzETMBEGA1UECBMKV2FzaGluZ3RvbjEQ
// SIG // MA4GA1UEBxMHUmVkbW9uZDEeMBwGA1UEChMVTWljcm9z
// SIG // b2Z0IENvcnBvcmF0aW9uMSUwIwYDVQQLExxNaWNyb3Nv
// SIG // ZnQgQW1lcmljYSBPcGVyYXRpb25zMSYwJAYDVQQLEx1U
// SIG // aGFsZXMgVFNTIEVTTjozQkJELUUzMzgtRTlBMTElMCMG
// SIG // A1UEAxMcTWljcm9zb2Z0IFRpbWUtU3RhbXAgU2Vydmlj
// SIG // ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB
// SIG // AKMUd7Vk9mgo7Bvk/cTv+0gdJq+K2F8x/ywmh3IltQsg
// SIG // eWtSdrFJ4LE3IUoHektxY53JAPIkJNFupGqYLRtJ4PUN
// SIG // 9PU8i/r8DocOxnc2DvPdCUOD8MSVeV9BpK+nfZ19P1Tv
// SIG // jY7R+8kLuwBMR6cFrPt+FNdCKXCO4dV9TlaptLEedNyI
// SIG // VamYBslz0E+4/7ulhcxSlLdrAYlmT1wEV9Vz0LWsDlyp
// SIG // PgK4gQxLDgdRCwibgQVsNMwFI9H+GktJWCgje6nVNM6n
// SIG // vqj2Aa0v6dBxV9VM0TZ9+zOkfEQID8WztevBxsQV7TTZ
// SIG // Ee9rRYE9uKRlpl4dAQcOLYRn0yWyPK0/av0CAwEAAaOC
// SIG // ARswggEXMB0GA1UdDgQWBBSOje7XFgYQflWTvzXCZPEx
// SIG // X/KgoDAfBgNVHSMEGDAWgBTVYzpcijGQ80N7fEYbxTNo
// SIG // WoVtVTBWBgNVHR8ETzBNMEugSaBHhkVodHRwOi8vY3Js
// SIG // Lm1pY3Jvc29mdC5jb20vcGtpL2NybC9wcm9kdWN0cy9N
// SIG // aWNUaW1TdGFQQ0FfMjAxMC0wNy0wMS5jcmwwWgYIKwYB
// SIG // BQUHAQEETjBMMEoGCCsGAQUFBzAChj5odHRwOi8vd3d3
// SIG // Lm1pY3Jvc29mdC5jb20vcGtpL2NlcnRzL01pY1RpbVN0
// SIG // YVBDQV8yMDEwLTA3LTAxLmNydDAMBgNVHRMBAf8EAjAA
// SIG // MBMGA1UdJQQMMAoGCCsGAQUFBwMIMA0GCSqGSIb3DQEB
// SIG // CwUAA4IBAQA8MeSrHFzkMg56ytF7PJuigV+XhDEaY0Jr
// SIG // pG7b6+AvbsLrBkHRHnyQGeJaXkCaMui8mUq8l4kk9p5i
// SIG // FTvBUd5fDHB/6RGRBi4YUXadGQg/x2XahvoB3jJvTB4P
// SIG // 2jpfmFJvpJpALIP9iOhKUPkCADgoKeybYC69rvMGbkQi
// SIG // Hp+J7ks6ozeueb84CXZQ6c1t98XmyDq9oXLjw8HUUwlR
// SIG // niCvdPbLyQReawT3v7ZmOMlRtwTWv6+z5vW/rFnB5/V1
// SIG // y0FNRaRbW/9r/taoxe00iOs4eMLCyweHAB+6UXuazgU7
// SIG // W/89g866wcbipsq8xGHEDgyehgPuHWHJJwp3G9Ss3eW5
// SIG // MIIGcTCCBFmgAwIBAgIKYQmBKgAAAAAAAjANBgkqhkiG
// SIG // 9w0BAQsFADCBiDELMAkGA1UEBhMCVVMxEzARBgNVBAgT
// SIG // Cldhc2hpbmd0b24xEDAOBgNVBAcTB1JlZG1vbmQxHjAc
// SIG // BgNVBAoTFU1pY3Jvc29mdCBDb3Jwb3JhdGlvbjEyMDAG
// SIG // A1UEAxMpTWljcm9zb2Z0IFJvb3QgQ2VydGlmaWNhdGUg
// SIG // QXV0aG9yaXR5IDIwMTAwHhcNMTAwNzAxMjEzNjU1WhcN
// SIG // MjUwNzAxMjE0NjU1WjB8MQswCQYDVQQGEwJVUzETMBEG
// SIG // A1UECBMKV2FzaGluZ3RvbjEQMA4GA1UEBxMHUmVkbW9u
// SIG // ZDEeMBwGA1UEChMVTWljcm9zb2Z0IENvcnBvcmF0aW9u
// SIG // MSYwJAYDVQQDEx1NaWNyb3NvZnQgVGltZS1TdGFtcCBQ
// SIG // Q0EgMjAxMDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCC
// SIG // AQoCggEBAKkdDbx3EYo6IOz8E5f1+n9plGt0VBDVpQoA
// SIG // goX77XxoSyxfxcPlYcJ2tz5mK1vwFVMnBDEfQRsalR3O
// SIG // CROOfGEwWbEwRA/xYIiEVEMM1024OAizQt2TrNZzMFcm
// SIG // gqNFDdDq9UeBzb8kYDJYYEbyWEeGMoQedGFnkV+BVLHP
// SIG // k0ySwcSmXdFhE24oxhr5hoC732H8RsEnHSRnEnIaIYqv
// SIG // S2SJUGKxXf13Hz3wV3WsvYpCTUBR0Q+cBj5nf/VmwAOW
// SIG // RH7v0Ev9buWayrGo8noqCjHw2k4GkbaICDXoeByw6ZnN
// SIG // POcvRLqn9NxkvaQBwSAJk3jN/LzAyURdXhacAQVPIk0C
// SIG // AwEAAaOCAeYwggHiMBAGCSsGAQQBgjcVAQQDAgEAMB0G
// SIG // A1UdDgQWBBTVYzpcijGQ80N7fEYbxTNoWoVtVTAZBgkr
// SIG // BgEEAYI3FAIEDB4KAFMAdQBiAEMAQTALBgNVHQ8EBAMC
// SIG // AYYwDwYDVR0TAQH/BAUwAwEB/zAfBgNVHSMEGDAWgBTV
// SIG // 9lbLj+iiXGJo0T2UkFvXzpoYxDBWBgNVHR8ETzBNMEug
// SIG // SaBHhkVodHRwOi8vY3JsLm1pY3Jvc29mdC5jb20vcGtp
// SIG // L2NybC9wcm9kdWN0cy9NaWNSb29DZXJBdXRfMjAxMC0w
// SIG // Ni0yMy5jcmwwWgYIKwYBBQUHAQEETjBMMEoGCCsGAQUF
// SIG // BzAChj5odHRwOi8vd3d3Lm1pY3Jvc29mdC5jb20vcGtp
// SIG // L2NlcnRzL01pY1Jvb0NlckF1dF8yMDEwLTA2LTIzLmNy
// SIG // dDCBoAYDVR0gAQH/BIGVMIGSMIGPBgkrBgEEAYI3LgMw
// SIG // gYEwPQYIKwYBBQUHAgEWMWh0dHA6Ly93d3cubWljcm9z
// SIG // b2Z0LmNvbS9QS0kvZG9jcy9DUFMvZGVmYXVsdC5odG0w
// SIG // QAYIKwYBBQUHAgIwNB4yIB0ATABlAGcAYQBsAF8AUABv
// SIG // AGwAaQBjAHkAXwBTAHQAYQB0AGUAbQBlAG4AdAAuIB0w
// SIG // DQYJKoZIhvcNAQELBQADggIBAAfmiFEN4sbgmD+BcQM9
// SIG // naOhIW+z66bM9TG+zwXiqf76V20ZMLPCxWbJat/15/B4
// SIG // vceoniXj+bzta1RXCCtRgkQS+7lTjMz0YBKKdsxAQEGb
// SIG // 3FwX/1z5Xhc1mCRWS3TvQhDIr79/xn/yN31aPxzymXlK
// SIG // kVIArzgPF/UveYFl2am1a+THzvbKegBvSzBEJCI8z+0D
// SIG // pZaPWSm8tv0E4XCfMkon/VWvL/625Y4zu2JfmttXQOnx
// SIG // zplmkIz/amJ/3cVKC5Em4jnsGUpxY517IW3DnKOiPPp/
// SIG // fZZqkHimbdLhnPkd/DjYlPTGpQqWhqS9nhquBEKDuLWA
// SIG // myI4ILUl5WTs9/S/fmNZJQ96LjlXdqJxqgaKD4kWumGn
// SIG // Ecua2A5HmoDF0M2n0O99g/DhO3EJ3110mCIIYdqwUB5v
// SIG // vfHhAN/nMQekkzr3ZUd46PioSKv33nJ+YWtvd6mBy6cJ
// SIG // rDm77MbL2IK0cs0d9LiFAR6A+xuJKlQ5slvayA1VmXqH
// SIG // czsI5pgt6o3gMy4SKfXAL1QnIffIrE7aKLixqduWsqdC
// SIG // osnPGUFN4Ib5KpqjEWYw07t0MkvfY3v1mYovG8chr1m1
// SIG // rtxEPJdQcdeh0sVV42neV8HR3jDA/czmTfsNv11P6Z0e
// SIG // GTgvvM9YBS7vDaBQNdrvCScc1bN+NR4Iuto229Nfj950
// SIG // iEkSoYICzjCCAjcCAQEwgfihgdCkgc0wgcoxCzAJBgNV
// SIG // BAYTAlVTMRMwEQYDVQQIEwpXYXNoaW5ndG9uMRAwDgYD
// SIG // VQQHEwdSZWRtb25kMR4wHAYDVQQKExVNaWNyb3NvZnQg
// SIG // Q29ycG9yYXRpb24xJTAjBgNVBAsTHE1pY3Jvc29mdCBB
// SIG // bWVyaWNhIE9wZXJhdGlvbnMxJjAkBgNVBAsTHVRoYWxl
// SIG // cyBUU1MgRVNOOjNCQkQtRTMzOC1FOUExMSUwIwYDVQQD
// SIG // ExxNaWNyb3NvZnQgVGltZS1TdGFtcCBTZXJ2aWNloiMK
// SIG // AQEwBwYFKw4DAhoDFQDoIgziqtIHOpyM14dSd14peCqr
// SIG // 6KCBgzCBgKR+MHwxCzAJBgNVBAYTAlVTMRMwEQYDVQQI
// SIG // EwpXYXNoaW5ndG9uMRAwDgYDVQQHEwdSZWRtb25kMR4w
// SIG // HAYDVQQKExVNaWNyb3NvZnQgQ29ycG9yYXRpb24xJjAk
// SIG // BgNVBAMTHU1pY3Jvc29mdCBUaW1lLVN0YW1wIFBDQSAy
// SIG // MDEwMA0GCSqGSIb3DQEBBQUAAgUA5EAx3TAiGA8yMDIx
// SIG // MDUwODA1MzgzN1oYDzIwMjEwNTA5MDUzODM3WjB3MD0G
// SIG // CisGAQQBhFkKBAExLzAtMAoCBQDkQDHdAgEAMAoCAQAC
// SIG // AhtJAgH/MAcCAQACAhEtMAoCBQDkQYNdAgEAMDYGCisG
// SIG // AQQBhFkKBAIxKDAmMAwGCisGAQQBhFkKAwKgCjAIAgEA
// SIG // AgMHoSChCjAIAgEAAgMBhqAwDQYJKoZIhvcNAQEFBQAD
// SIG // gYEASIf6VW0iPdqMdQOQyC/jNfg1OXbFQ1KAi7sfEFAV
// SIG // FatQui1JbweC6AQ1+NyEX2p2Li+VsYADVANfWJo21p1d
// SIG // ibQMnmZRUWWXrBYCOkSAJueLIp5uOsIqX3PH8jQCz2sv
// SIG // snjaYD3CmQk/5NOmJTxho3Hka4Rs6I3j0q3VlO54ZCgx
// SIG // ggMNMIIDCQIBATCBkzB8MQswCQYDVQQGEwJVUzETMBEG
// SIG // A1UECBMKV2FzaGluZ3RvbjEQMA4GA1UEBxMHUmVkbW9u
// SIG // ZDEeMBwGA1UEChMVTWljcm9zb2Z0IENvcnBvcmF0aW9u
// SIG // MSYwJAYDVQQDEx1NaWNyb3NvZnQgVGltZS1TdGFtcCBQ
// SIG // Q0EgMjAxMAITMwAAAU9kLnX2egNagwAAAAABTzANBglg
// SIG // hkgBZQMEAgEFAKCCAUowGgYJKoZIhvcNAQkDMQ0GCyqG
// SIG // SIb3DQEJEAEEMC8GCSqGSIb3DQEJBDEiBCCjn9Y37LSV
// SIG // QdbqjwO3huALTsPxk6zoIxDB+25XU753PzCB+gYLKoZI
// SIG // hvcNAQkQAi8xgeowgecwgeQwgb0EIABnJhD2glikwMZy
// SIG // IW0YcnpDz6r4eW0wjRDFxsEAtTcCMIGYMIGApH4wfDEL
// SIG // MAkGA1UEBhMCVVMxEzARBgNVBAgTCldhc2hpbmd0b24x
// SIG // EDAOBgNVBAcTB1JlZG1vbmQxHjAcBgNVBAoTFU1pY3Jv
// SIG // c29mdCBDb3Jwb3JhdGlvbjEmMCQGA1UEAxMdTWljcm9z
// SIG // b2Z0IFRpbWUtU3RhbXAgUENBIDIwMTACEzMAAAFPZC51
// SIG // 9noDWoMAAAAAAU8wIgQgDkDaphAYLgTqo5XgJWPbuvpy
// SIG // Y6sMGTB7Viiat2n+MNYwDQYJKoZIhvcNAQELBQAEggEA
// SIG // QbnUMgh6ZXg3tuuICEPdPEYL6E9XEE8JfFSibkbyH98Z
// SIG // 3LxV+cxCcRJ0RuM8EoIirIcP5hOLS288pENmpIkSAoS+
// SIG // Anvy6zxCe7xgCEYxRj6BHxznCOah1KMW6vK8EENuKNlL
// SIG // 1PIm1zgtiK88hfuAiEtaZ/+8rHZl3nT2Go22MXMiFpLz
// SIG // 8WzE6SHCUm3rdtOI4i0mtPA0BilqcDj5nbWubiQtGHCU
// SIG // BRb6ErjqEATgNJKLzFKIuOKuY70TkD97K94GpV9zdvrf
// SIG // /Y972+7nxTEKfeGCPGkzZnqNNcLjJFWGLfSp3QLjzdP3
// SIG // 2F8DPZQqn53nk3D24A/W84RPaBRg6x+nMg==
// SIG // End signature block
