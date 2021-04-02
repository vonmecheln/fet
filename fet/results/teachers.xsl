<xsl:stylesheet version='1.0' xmlns:xsl='http://www.w3.org/1999/XSL/Transform'>
<xsl:template match="Teacher">
<table border="1" cellpadding="6">
<tr>
	<th colspan="10">
	<xsl:value-of select="@name" />
	</th>
</tr>
<tr>
<td>Emploi du temps</td>
	<xsl:for-each select="/*/Teacher[1]/Day[1]/*">
		<xsl:call-template name="header" />
	</xsl:for-each>
</tr>
	<xsl:for-each select="child::*">
<tr>
		<xsl:call-template name="days" />
</tr>
	</xsl:for-each>
</table>
</xsl:template>

<xsl:template name="header">
<td>	<xsl:value-of select="@name" /></td>
</xsl:template>

<xsl:template name="days">

<td><xsl:value-of select="@name" /></td>
	<xsl:for-each select="child::*">
<td style="width:14em;">
		<xsl:for-each select="child::*">
			<xsl:for-each select="child::*">
				<xsl:value-of select="@name" /><br />
			</xsl:for-each>
		</xsl:for-each>
</td>
	</xsl:for-each>
</xsl:template>
</xsl:stylesheet>
