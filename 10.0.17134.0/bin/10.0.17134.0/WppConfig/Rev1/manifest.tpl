<?xml version="1.0" standalone="yes"?>
<!--
`**********************************************************************`
`* This is a template file for the tracewpp preprocessor.             *`
`* If you need to use a custom version of this file in your project,  *`
`* please clone it from this one and point WPP to it by specifying    *`
`* -gen:{yourfile}*.tmh on the RUN_WPP line in your sources file.     *`
`*                                                                    *`
`*    Copyright (c) Microsoft Corporation. All rights reserved.       *`
`**********************************************************************`
Checksum="`Compiler.Checksum`" Generated file. Do not edit.
File created by `Compiler.Name` compiler version `Compiler.Version`
from template `TemplateFile`

***************************************************************************
PREVIEW: Behavior of the -man option, the `TemplateFile` template, and the
associated configuration options may change in future versions of tracewpp.
***************************************************************************
-->
<instrumentationManifest
    xmlns="http://schemas.microsoft.com/win/2004/08/events"
	xmlns:xs="http://www.w3.org/2001/XMLSchema"
    xmlns:win="http://manifests.microsoft.com/win/2004/08/windows/events"
	>
  <instrumentation>
    <events>
`FORALL ControlGuid IN ControlGuids`
`  FORALL TraceGuid IN ControlGuid.TraceGuids`
	  <provider
		  name="`ControlGuid.Name`_`TraceGuid.Normalized`"
		  controlGuid="{`ControlGuid.Text`}"
		  guid="{`TraceGuid.Text`}"
          symbol="`ControlGuid.Symbol``IF TraceGuid No`_`TraceGuid.No``ENDIF`"
          resourceFileName="`Manifest.ResourceFileName`"
          messageFileName="`Manifest.MessageFileName`"
		  >
`    IF ControlGuid Levels`
		<levels>
`      FORALL Level IN ControlGuid.Levels`
          <level value="`Level.Value`" name="`Level.Name`" symbol="`Level.Symbol`" />
`      ENDFOR Level`
		</levels>
`    ENDIF ControlGuid Levels`
`    IF ControlGuid Keywords`
		<keywords>
`    FORALL Keyword IN ControlGuid.Keywords`
          <keyword mask="`Keyword.Mask`" name="`Keyword.Name`" symbol="`Keyword.Symbol`" />
`    ENDFOR Keyword`
		</keywords>
`    ENDIF ControlGuid Keywords`
        <events>
`    FORALL Message IN TraceGuid.Messages`
          <event
`      IF Message EventName`
              name="`Message.EventName`"
`      ENDIF Message EventName`
              attributes='`Message.Attributes`'
              value="`Message.MsgNo`"
			  level="`Message.Level`"
			  keywords="`Message.Keywords`"
              symbol="`Message.Name`"
              message="$(string.event.`Message.GuidNo`_`Message.MsgNo`)"
`      IF Message Arguments`
              template="T_`Message.GuidNo`_`Message.MsgNo`"
`      ENDIF Message Arguments`
              />
`    ENDFOR Message`
        </events>
        <templates>
`    FORALL Message IN TraceGuid.Messages`
`      IF Message Arguments`
          <template tid="T_`Message.GuidNo`_`Message.MsgNo`">
`        FORALL Argument IN Message.Arguments`
            <data
                name="`Argument.Name`"
                inType="`Argument.InType`"
`          IF Argument OutType`
                outType="`Argument.OutType`"
`          ENDIF Argument OutType`
                />
`        ENDFOR Argument`
          </template>
`      ENDIF Message Arguments`
`    ENDFOR Message`
        </templates>
	  </provider>
`  ENDFOR TraceGuid`
`ENDFOR ControlGuid`
    </events>
  </instrumentation>
  <localization>
    <resources culture="`Manifest.Culture`">
      <stringTable>
`FORALL Message IN Messages`
        <string
			id="event.`Message.GuidNo`_`Message.MsgNo`"
			value="`Message.ManifestText`"
		    />
`ENDFOR Message`
      </stringTable>
    </resources>
  </localization>
</instrumentationManifest>
