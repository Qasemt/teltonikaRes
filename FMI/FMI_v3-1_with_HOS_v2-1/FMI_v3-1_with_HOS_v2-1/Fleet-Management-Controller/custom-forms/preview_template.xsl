<?xml version="1.0" encoding="utf-8"?>
<!-- render a form template as an HTML form -->
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="html" encoding="utf-8" indent="yes" />
    <!-- begin runtime params -->
    <xsl:param name="resources">..\..\..\</xsl:param>
    <!-- end runtime params -->
    <xsl:template match="form">
        <xsl:text disable-output-escaping="yes">&lt;!DOCTYPE html></xsl:text>
        <html>
            <head>
                <title><xsl:value-of select="title"/></title>
                <link href="{$resources}custom_forms.css" rel="stylesheet"/>
                <link href="{$resources}../external/bootstrap/css/bootstrap.min.css" rel="stylesheet" media="screen"/>
                <link href="{$resources}../external/jquery-ui/css/jquery-ui-bootstrap/jquery-ui-1.8.16.custom.css" rel="stylesheet"/>
                <link href="{$resources}../external/jquery.ui.timepicker.css" rel="stylesheet"/>
            </head>
            <body>
                <div class="header">
                <h2 style="float: left;">
                    <xsl:value-of select="title"/>
                </h2>
                <a href="#" class="btn" style="float: right;" onclick="history.back();">Back</a>
                </div>
                <div style="clear: both;"></div>
                <!-- list each item in table of contents -->
                <!-- following an item link will display a form with that item's representation similar to how it is on the device -->
                <ul id="tableOfContents">
                    <xsl:for-each select="item">
                        <li id="toc-field-{@id}"><a class="mock" onclick="jumpTo('#field-{@id}',event);"><h2><xsl:value-of select="title"/></h2><span class="item-value"><xsl:value-of select="default"/></span></a></li>
                    </xsl:for-each>
                </ul>
                <form>
                    <xsl:apply-templates select="item"/>
                </form>
                <script src="{$resources}../external/jquery-1.7.2.min.js"></script>
                <script src="{$resources}../external/jquery-ui/js/jquery-ui-1.8.21.custom.min.js"></script>
                <script src="{$resources}../external/jquery.ui.timepicker.js"></script>
                <script src="{$resources}../external/bootstrap/js/bootstrap.min.js"></script>
                <script src="{$resources}custom_forms.js"></script>
                <script type="text/javascript">
                    initializeForm();
                </script>
            </body>
        </html>
    </xsl:template>

    <!-- render basic item info and then dispatch to the item subclass renderer template -->
    <xsl:template match="item">
        <fieldset id="field-{@id}">
            <legend>
                <xsl:value-of select="title"/>
            </legend>
            <div>
            <xsl:apply-templates select="keyboard-type"/>
            <xsl:apply-templates select="int-numpad-type"/>
            <xsl:apply-templates select="single-select-type"/>
            <xsl:apply-templates select="multiple-select-type"/>
            <xsl:apply-templates select="date-type"/>
            <xsl:apply-templates select="time-type"/>
            <xsl:apply-templates select="stops-list-type"/>
            </div>
            <button class="btn btn-cancel">Cancel</button>
            <button class="btn btn-primary">Save</button>
        </fieldset>
    </xsl:template>

    <xsl:template match="keyboard-type">
        <input class="input-large" id="{../@id}" type="text" placeholder="{placeholder-text}" maxlength="{@max-text-len}"/>
    </xsl:template>

    <xsl:template match="int-numpad-type">
        <input class="input-large" id="{../@id}" type="number" placeholder="{placeholder-text}" maxlength="{@max-int-len}" min="{@min-int-val}" max="{@max-int-val}"/>
    </xsl:template>

    <!-- render as a radio button group, similar to the device -->
    <xsl:template match="single-select-type">
        <ul class="select-list single" id="{../@id}">
        <xsl:for-each select="option">
            <li><label class="input radio"><input type="radio" name="single-select{../../@id}" value="{@id}"/> <xsl:value-of select="."/></label></li>
        </xsl:for-each>
        </ul>
    </xsl:template>

    <!-- render as a list of checkboxes, similar to the device -->
    <xsl:template match="multiple-select-type">
        <ul class="select-list multiple" id="{../@id}">
        <xsl:for-each select="option">
            <li><label class="input checkbox"><input type="checkbox" name="{@id}"/> <xsl:value-of select="."/></label></li>
        </xsl:for-each>
        </ul>
    </xsl:template>

    <xsl:template match="date-type">
        <input class="date" id="{../@id}" type="text"/>
    </xsl:template>

    <xsl:template match="time-type">
        <input class="time" id="{../@id}" type="text"/>
    </xsl:template>

    <xsl:template match="stops-list-type">
        <ul class="select-list stops" id="{../@id}">
            <li><label class="input radio"><input type="radio" name="stops-list1" value="1"/> Sample Stop 1</label></li>
            <li><label class="input radio"><input type="radio" name="stops-list1" value="2"/> Sample Stop 2</label></li>
            <li><label class="input radio"><input type="radio" name="stops-list1" value="3"/> Sample Stop 3</label></li>
        </ul>
    </xsl:template>

</xsl:stylesheet>