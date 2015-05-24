<?xml version="1.0" encoding="utf-8"?>
<!-- render a submitted form as HTML -->
<!-- this transform depends on the associated form template file, which either needs to be specified via transformation parameters,
        or exist 3 folders up from the submit file being transformed (due to the way the custom forms library is structured) -->
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="html" encoding="utf-8" indent="yes" />
    <!-- begin runtime params -->
    <!-- this param can be specified by the transformer, in case the form template is not in same folder -->
    <!--<xsl:param name="formPath"></xsl:param>-->
    <xsl:param name="formPath"></xsl:param>
    <xsl:param name="resources">../../../</xsl:param>
    <!-- end runtime params -->
    <xsl:variable name="tmplt" select="document(string(concat($formPath,'form.xml')))/form"/>
    <xsl:template match="form">
        <xsl:text disable-output-escaping="yes">&lt;!DOCTYPE html></xsl:text>
        <html>
            <head>
                <title><xsl:value-of select="title"/></title>
                <link href="{$resources}custom_forms.css" rel="stylesheet"/>
                <link href="{$resources}../external/bootstrap/css/bootstrap.min.css" rel="stylesheet" media="screen"/>
            </head>
            <body>
                <div class="header">
                <h1 style="float: left;">
                    <xsl:value-of select="$tmplt/title"/>
                </h1>
                <a href="#" class="btn" style="float: right;" onclick="history.back();">Back</a>
                </div>
                <div style="clear: both;"></div>
                <div id="submitted-values">
                <ul>
                    <xsl:apply-templates select="item"/>
                </ul>
                </div>
                <script src="{$resources}../external/jquery-1.7.2.min.js"></script>
                <script src="{$resources}../external/bootstrap/js/bootstrap.min.js"></script>
            </body>
        </html>
    </xsl:template>

    <!-- render basic item info and then dispatch to the item subclass renderer template -->
    <xsl:template match="item">
        <li>
        <xsl:variable name="id" select="@id"/>
        <h2>
            <xsl:value-of select="$tmplt/item[@id = $id]/title"/>
        </h2>
        <xsl:variable name="elem-type" select="local-name(*[1])"/>
        <xsl:if test="not($tmplt/item[@id = $id][child::*[local-name()=$elem-type]])">
            type mismatch: <xsl:value-of select="$elem-type"/> in submitted form is incompatible with element associated by the id <xsl:value-of select="$id"/> in the form template.
        </xsl:if>
        <div>
        <xsl:apply-templates select="keyboard-type"/>
        <xsl:apply-templates select="int-numpad-type"/>
        <xsl:apply-templates select="single-select-type"/>
        <xsl:apply-templates select="multiple-select-type"/>
        <xsl:apply-templates select="date-type"/>
        <xsl:apply-templates select="time-type"/>
        <xsl:apply-templates select="stops-list-type"/>
        </div>
        </li>
    </xsl:template>

    <xsl:template match="keyboard-type">
        <xsl:variable name="item-id" select="../@id"/>
        <label class="text"><xsl:value-of select="."/></label>
    </xsl:template>

    <xsl:template match="int-numpad-type">
        <label class="number"><xsl:value-of select="."/></label>
    </xsl:template>

    <!-- dispaly each option and highlight the one that was selected -->
    <xsl:template match="single-select-type">
        <xsl:variable name="item-id" select="../@id"/>
        <xsl:variable name="selected" select="option"/>
        <ul class="select-list single" id="{../@id}">
        <xsl:for-each select="$tmplt/item[@id = $item-id]/single-select-type/option">
            <xsl:choose>
            <xsl:when test="@id = $selected/@id">
                <li class="selected"><label class="input radio"><xsl:value-of select="."/></label></li>
            </xsl:when>
            <xsl:otherwise>
                <li class="unselected"><label class="input radio"><xsl:value-of select="."/></label></li>
            </xsl:otherwise>
            </xsl:choose>
        </xsl:for-each>
        </ul>
    </xsl:template>

    <!-- dispaly each option and highlight each that were selected -->
    <xsl:template match="multiple-select-type">
        <xsl:variable name="item-id" select="../@id"/>
        <xsl:variable name="selected" select="option"/>
        <ul class="select-list multiple" id="{../@id}">
        <xsl:for-each select="$tmplt/item[@id = $item-id]/multiple-select-type/option">
            <xsl:choose>
            <xsl:when test="@id = $selected/@id">
                <li class="selected"><label class="input checkbox"><xsl:value-of select="."/></label></li>
            </xsl:when>
            <xsl:otherwise>
                <li class="unselected"><label class="input checkbox"><xsl:value-of select="."/></label></li>
            </xsl:otherwise>
            </xsl:choose>
        </xsl:for-each>
        </ul>
    </xsl:template>

    <xsl:template match="date-type">
        <label class="date"><xsl:value-of select="@month"/>/<xsl:value-of select="@day"/>/<xsl:value-of select="@year"/></label>
    </xsl:template>

    <xsl:template match="time-type">
        <label class="time"><xsl:value-of select="format-number(@hour, '00')"/>:<xsl:value-of select="format-number(@minute, '00')"/></label>
    </xsl:template>

    <xsl:template match="stops-list-type">selected stop id: <xsl:value-of select="@id"/></xsl:template>

</xsl:stylesheet>