<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
	xmlns:html="http://www.w3.org/1999/xhtml"
	xmlns:dt="urn:schemas-microsoft-com:datatypes">
<xsl:output method="html"/>

<!--	===============================================
			Definitions of Global Variables
		=============================================== -->
	
<!-- List of Application tables -->
<xsl:variable name="app_table_list" select="*/table[
		(@name!='_FOREIGN_KEYS') and
		(@name!='_STORED_PROCEDURES') and
		(@name!='_COLUMNS_INFO') and
		(@name!='_TABLES_INFO') and
		(@name!='_SEQUENCE_GENERATORS')]"/>

<!-- Foreign Keys and Tables Info tables -->
<xsl:variable name="foreign_keys" select="*/table[@name='_FOREIGN_KEYS'][position()=1]"/>
<xsl:variable name="tables_info" select="*/table[@name='_TABLES_INFO'][position()=1]"/>

<!--	===============================================
			Root Template
		=============================================== -->

<xsl:template match="/">
<html>
	<head>
		<xsl:call-template name="Scripts"/>
		<xsl:call-template name="CSS_Style"/>
		<title>Media Info Database</title>
	</head>

	<body>

		<!-- Document Title -->
		<xsl:call-template name="doc_title"/>

		<!-- Summary Info -->
		<xsl:apply-templates select="*/summary"/>

		<!-- Table of Contents -->
		<a name="___TOC___"/>
		<xsl:call-template name="TableOfContents"/>

		<!-- Tables -->
		<xsl:apply-templates select="$app_table_list"/>

	</body>
</html>
</xsl:template>

<!--	===============================================
			Document Title Template
		=============================================== -->

<xsl:template name="doc_title">
	<table border="0" cellpadding="0" cellspacing="0">
		<tr valign="middle">
			<td>
				<h1>
					<xsl:choose>
						<xsl:when test="//summary/title!=''">
							<xsl:value-of select="//summary/title"/>
						</xsl:when>
						<xsl:otherwise>
							CD/DVD Content
						</xsl:otherwise>
					</xsl:choose>
					<xsl:if test="//summary/subject!=''">
							<br/><xsl:value-of select="//summary/subject"/>
					</xsl:if>
				</h1>
			</td>
		</tr>
	</table>
	<div class="delimiter"/>
</xsl:template>

<!--	===============================================
			Summary Information Template
		=============================================== -->

<xsl:template match="*/summary">
	<h2>Summary Information</h2>
	<table>
		<xsl:if test="//summary/createdtm!=''">
			<tr><td width="20"/><td><b>Created Date/Time:</b></td><td/><td><xsl:value-of select="//summary/createdtm"/></td></tr>
		</xsl:if>
		<xsl:if test="//summary/comments!=''">
			<tr><td width="20"/><td><b>Comments:</b></td><td/><td><xsl:value-of select="//summary/comments"/></td></tr>
		</xsl:if>
		<xsl:if test="//summary/appname!=''">
			<tr><td width="20"/><td><b>Application:</b></td><td/><td><xsl:value-of select="//summary/appname"/></td></tr>
		</xsl:if>
	</table>
</xsl:template>

<!--	===============================================
			Table Of Contents Template
		=============================================== -->

<xsl:template name="TableOfContents">

	<!-- List of Application Tables -->
	<xsl:if test="$app_table_list!=''">
		<h2>Application Tables</h2>
		<table>
			<xsl:for-each select="$app_table_list">
				<xsl:call-template name="TableOfContentsRecord"/>
			</xsl:for-each>
		</table>
	</xsl:if>

</xsl:template>

<!--	===============================================
			Table of Contents Record Template
		=============================================== -->

<xsl:template name="TableOfContentsRecord">
	<xsl:variable name="table_name" select="@name"/>
	<tr>
		<td width="20"/>
		<td>
			<a>
				<xsl:attribute name="href">#<xsl:value-of select="$table_name"/></xsl:attribute>
				<xsl:value-of select="$table_name"/>
			</a>
		</td>
		<td width="10"/>
		<td>
			<a>
				<xsl:attribute name="href">#<xsl:value-of select="$table_name"/></xsl:attribute>
				<xsl:value-of select="$tables_info/row[td[1]=$table_name]/td[2]"/>
			</a>
		</td>
	</tr>
</xsl:template>

<!--	===============================================
			Table Template
		=============================================== -->

<xsl:template match="*/table">
	<xsl:call-template name="TableTitle"/>
	<table class="ex" cellspacing="0" cellpadding="0">
		<xsl:call-template name="TableHeader"/>
		<xsl:for-each select="row">
			<tr>
				<xsl:call-template name="TableRow"/>
			</tr>
		</xsl:for-each>
	</table>
</xsl:template>

<!--	===============================================
			Table Title Template
		=============================================== -->

<xsl:template name="TableTitle">
	<xsl:variable name="table_name" select="@name"/>
	<a>
		<xsl:attribute name="name">
			<xsl:value-of select="$table_name"/>
		</xsl:attribute>
	</a>
	<div class="delimiter"/>

	<!-- Table Links (Previous | Table of Contents | Next) -->
	<p class="tbl_links">
		<xsl:if test="preceding-sibling::table[@name!='_TABLES_INFO']">
			<a>
				<xsl:attribute name="href">
					#<xsl:value-of select="preceding-sibling::table[@name!='_TABLES_INFO'][position()=1]/@name"/>
				</xsl:attribute>
				Previous
			</a>
			<xsl:text> | </xsl:text>
		</xsl:if>

		<a href="#___TOC___">Table of Contents</a>

		<xsl:if test="following-sibling::table[@name!='_TABLES_INFO']">
			<xsl:text> | </xsl:text>
			<a>
				<xsl:attribute name="href">
					#<xsl:value-of select="following-sibling::table[@name!='_TABLES_INFO'][position()=1]/@name"/>
				</xsl:attribute>
				Next
			</a>
		</xsl:if>
	</p>

	<!-- Table title -->
	<h2>
		<xsl:value-of select="$table_name"/>
		<xsl:variable name="table_title" select="$tables_info/row[td[1]=$table_name]/td[2]"/>
		<xsl:if test="$table_title!=''">
			- <xsl:value-of select="$table_title"/>
		</xsl:if>
	</h2>

	<!-- Short description of the table (Table Remarks) -->
	<xsl:variable name="table_info" select="$tables_info/row[td[1]=$table_name]/td[4]"/>
	<xsl:if test="$table_info!=''">
		<p class="tbl_info">
			<xsl:value-of select="$table_info"/>
		</p>
	</xsl:if>

</xsl:template>

<!--	===============================================
			Table Column Header Template
		=============================================== -->

<xsl:template name="TableHeader">
	<xsl:for-each select="col">
		<!-- 'Remarks' column of _COLUMNS_INFO table is not included into document -->
		<xsl:if test="(ancestor::table/@name!='_COLUMNS_INFO') or (position() &lt; 9)">
		  <th>
				<xsl:attribute name="OnMouseOver">
					showTip( '<xsl:value-of select="ancestor::table/@name"/>','<xsl:value-of select="."/>')
				</xsl:attribute>
				<xsl:choose>
					<xsl:when test="@key='yes'">
						<b><xsl:value-of select="."/></b>
						<br><xsl:value-of select="@def"/> (key)</br>
					</xsl:when>
					<xsl:otherwise>
						<xsl:value-of select="."/>
						<br><xsl:value-of select="@def"/></br>
					</xsl:otherwise>
				</xsl:choose>
			</th>
		</xsl:if>
	</xsl:for-each>
</xsl:template>

<!--	===============================================
			Table Row Template
		=============================================== -->

<xsl:template name="TableRow">
	<xsl:variable name="table_name" select="ancestor::table/@name"/>
	<xsl:for-each select="td">
		<xsl:variable name="pos" select="position()"/>
		<xsl:variable name="col" select="ancestor::table/col[$pos]"/>
		<xsl:variable name="foreign_keys_row" select="$foreign_keys/row[(td[1]=$table_name) and (td[2]=$col)][position()=1]"/>

		<td>
			<xsl:attribute name="OnMouseOver">
				showTip( '<xsl:value-of select="$table_name"/>','<xsl:value-of select="$col"/>')
			</xsl:attribute>
			<xsl:if test="$col/@key='yes'">
				<xsl:attribute name="class">key</xsl:attribute>
			</xsl:if>
			<xsl:choose>
				<xsl:when test=".=''">
					<i>Null</i>
				</xsl:when>

				<xsl:when test="$foreign_keys_row!=''">
					<a>
						<xsl:attribute name="href">#<xsl:value-of select="$foreign_keys_row/td[3]"/></xsl:attribute>
						<xsl:value-of select="."/>
					</a>
				</xsl:when>

				<xsl:when test="($table_name='InstallationPackage') and ($pos=10)">
					<a>
						<xsl:attribute name="href">.<xsl:value-of select="."/></xsl:attribute>
						<xsl:attribute name="target">_blank</xsl:attribute>
						<xsl:value-of select="."/>
					</a>
				</xsl:when>

				<xsl:when test="($table_name='AppTemplate') and ($pos=3)">
					<a>
						<xsl:attribute name="href">.<xsl:value-of select="."/></xsl:attribute>
						<xsl:attribute name="target">_blank</xsl:attribute>
						<xsl:value-of select="."/>
					</a>
				</xsl:when>

				<xsl:when test="($table_name='AppMetadata') and ($pos=3)">
					<a>
						<xsl:attribute name="href">.<xsl:value-of select="."/></xsl:attribute>
						<xsl:attribute name="target">_blank</xsl:attribute>
						<xsl:value-of select="."/>
					</a>
				</xsl:when>

				<xsl:otherwise>
					<xsl:value-of select="."/>
				</xsl:otherwise>

			</xsl:choose>
		</td>
	</xsl:for-each>
</xsl:template>

<!--	===============================================
			Scripts Template
		=============================================== -->

<xsl:template name="Scripts">
	<script language="JavaScript" type="text/javascript">
	<![CDATA[ <!--

		// showTip() - Show tip about table column.
		// Function display info from _COLUMNS_INFO or _FOREIGN_KEYS tables
		function showTip( table_name, column_name)
		{
			var tip_msg = "";
			var node = document.XMLDocument.selectSingleNode(
				"*/table[@name='_COLUMNS_INFO']/row[td[0]='" + table_name +
				"' and td[1]='" + column_name + "']/td[8]");
			if (node != null)
			{
				tip_msg = node.nodeTypedValue;
			}
			if (tip_msg == "")
			{
				node = document.XMLDocument.selectSingleNode(
					"*/table[@name='_FOREIGN_KEYS']/row[td[0]='" + table_name +
					"' and td[1]='" + column_name + "']/td[2]");
				if (node != null)
				{
					tip_msg = "Foreign key: " + node.nodeTypedValue + " table";
					node = document.XMLDocument.selectSingleNode(
						"*/table[@name='_FOREIGN_KEYS']/row[td[0]='" + table_name +
						"' and td[1]='" + column_name + "']/td[3]");
					tip_msg += ", " + node.nodeTypedValue + " column";
				}
			}
			window.event.srcElement.title = tip_msg;
			return true;
		}
	--> ]]>
	</script>
</xsl:template>

<!--	===============================================
			CSS Styles Template
		=============================================== -->

<xsl:template name="CSS_Style">
	<style type="text/css">
		*
		{
			margin: 0px 0px 0px 0px;
			font-family: Arial, Helvetica, sans-serif;
			font-size: 12px;
			color: #000000;
		}
		body
		{
			margin: 0px 20px 40px 40px;
			padding: 0;
		}
		p
		{
			padding: 8px 0px 0px 0px;
		}
		p.copyright
		{
			height: 20px;
			margin: 12px 0px 0px 0px;
			padding: 2px 0px 0px 10px;
			background-color: #001850;
			text-align: left;
			color: #FFFFFF;
		}
		p.tbl_info
		{
			width: 640px;
			padding: 0px 0px 20px 0px;
		}
		p.tbl_links
		{
			padding: 0px 0px 0px 0px;
		}
		h1, h2
		{
			font-weight: normal;
			text-align: left;
			color: #001850;
		}
		h1
		{
			font-size: 24px;
			margin: 12px 0px 12px 0px;
		}
		h2
		{
			margin: 20px 0px 10px 0px;
			padding: 0px 0px 0px 0px;
			font-size: 18px;
		}
		a
		{
			text-decoration: none;
			color: #0000CC;
		}
		.delimiter
		{
			height: 2px;
			margin: 25px 0px 0px 0px;
			background-color: #001850;
			font-size: 2px;
		}
		table.ex
		{
			border-top: 1px solid #808080;
			border-left: 1px solid #808080;
			border-bottom: 0px solid #808080;
			border-right: 0px solid #808080;
		}
		table.ex th, table.ex td 
		{
			padding: 3px 5px 3px 5px;
			border-top: 0px solid #808080;
			border-left: 0px solid #808080;
			border-bottom: 1px solid #808080;
			border-right: 1px solid #808080;
		}
		table.ex th
		{
			font-weight: normal;
			background-color: #F0F0F0;
		}
		table.ex td.key
		{
			font-weight: normal;
			background-color: #FBFBFB;
		}

	</style>
</xsl:template>
</xsl:stylesheet>
