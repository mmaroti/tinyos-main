<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<!--<xsl:output indent="yes" method="xml" media-type="text/xhtml" omit-xml-declaration="yes" /> -->
		
<xsl:template match="/">
<html>

<head>
<meta name="generator" content="Benchmark result report" />
<meta name="robots" content="noindex,nofollow" />
<meta http-equiv="content-type" content="text/html; charset=UTF-8" />
<meta http-equiv="description" content="RadioTest result report" />
<title>RadioTest result report</title>
<link rel="stylesheet" href="assets/benchmark.css" type="text/css"/>
</head>

<body>
<div class="top"></div>
<xsl:apply-templates/>
</body>
</html>
</xsl:template>

<xsl:template match="testresult">
<table class="testresult">
  <tr valign="top">

    <td width="20%">
      <div class="title">Testcase</div>
      <xsl:apply-templates select="configuration"/>
    </td>

    <td>
      <div class="title">Statistics (<xsl:value-of select="@date"/>)</div>
      <table class="data">
        <tr>  
          <td class="sub"></td>
          <td class="title_send" colspan="4">&#160;</td>
          <td class="title_send" colspan="3">send()</td>
          <td class="title_send" colspan="3">sendDone()</td>
          <td class="title_send" colspan="2">&#160;</td>
          <td class="title_rcv" colspan="6">receive()</td>
        </tr>
        <tr>
          <td class="sub"><strong>Edge</strong></td>
          <td class="sub">Rem</td>
          <td class="sub">Triggered</td>
          <td class="sub">Backlog</td>
          <td class="sub">Resend</td>

          <td class="sub">Total</td>
          <td class="sub">SUCC</td>
          <td class="sub">FAIL</td>

          <td class="sub">Total</td>
          <td class="sub">SUCC</td>
          <td class="sub">FAIL</td>

          <td class="sub">ACK</td>
          <td class="sub">noACK</td>
 
          <td class="sub">Total</td>
          <td class="sub">Expected</td>
          <td class="sub">Wrong</td>
          <td class="sub">Duplicate</td>
          <td class="sub">Forward</td>
          <td class="sub">Missed</td>

        </tr>
        <xsl:for-each select="statlist/stat">
        <tr>
          <xsl:variable name="curr_idx" select="@idx"/>
          <td class="sub"><xsl:value-of select="$curr_idx"/></td>
          <xsl:apply-templates select="current()[@idx=$curr_idx]"/>
        </tr>
        </xsl:for-each>
      </table>

<!-- ERROR CHECKING -->

      <table class="data">
      <tr>
        <td class="sub"><strong>Edge</strong></td>
        <td class="sub">Send (S/F)</td>
        <td class="sub">sDone(S/F)</td>
        <td class="sub">ACK</td>
        <td class="sub">resendC</td>
        <td class="sub">Recv (E/W)</td>
        <td class="sub">Wrong (Dupl/Fwd)</td>
      </tr>
      <xsl:for-each select="statlist/stat">
        <tr>
          <td class="sub"><xsl:value-of select="@idx"/></td>

          <td class="sub">
          <xsl:choose>
            <xsl:when test="SC - SSC - SFC = 0"><span class="debug_ok">OK</span>
            </xsl:when>
            <xsl:otherwise><span class="debug_fail">ERR</span>
            </xsl:otherwise>
          </xsl:choose>
          </td>
          <td class="sub">
          <xsl:choose>
            <xsl:when test="SDC - SDSC - SDFC = 0"><span class="debug_ok">OK</span>
            </xsl:when>
            <xsl:otherwise><span class="debug_fail">ERR</span>
            </xsl:otherwise>
          </xsl:choose>
          </td>
          <td class="sub">
          <xsl:choose>
            <xsl:when test="../../configuration/ack = 'Off'"><span class="debug_ok">-</span>
            </xsl:when>
            <xsl:otherwise>
              <xsl:choose>
              <xsl:when test="SDSC - WAC - NAC = 0"><span class="debug_ok">OK</span>
              </xsl:when>
              <xsl:otherwise><span class="debug_fail">ERR</span>
              </xsl:otherwise>
              </xsl:choose>
            </xsl:otherwise>
          </xsl:choose>
          </td>
          <td class="sub">
          <xsl:choose>
            <xsl:when test="RC - SFC - SDFC - NAC = 0"><span class="debug_ok">OK</span>
            </xsl:when>
            <xsl:otherwise><span class="debug_fail">ERR</span>
            </xsl:otherwise>
          </xsl:choose>
          </td>

          <td class="sub">
          <xsl:choose>
            <xsl:when test="RCC - EXC - WC = 0"><span class="debug_ok">OK</span>
            </xsl:when>
            <xsl:otherwise><span class="debug_fail">ERR</span>
            </xsl:otherwise>
          </xsl:choose>
          </td>

          <td class="sub">
          <xsl:choose>
            <xsl:when test="WC - DRC - FC = 0"><span class="debug_ok">OK</span>
            </xsl:when>
            <xsl:otherwise><span class="debug_fail">ERR</span>
            </xsl:otherwise>
          </xsl:choose>
          </td>

        </tr>
      </xsl:for-each>
      </table>

<!-- END ERROR CHECKING -->

      <xsl:apply-templates select="debuglist"/>
      <xsl:apply-templates select="errorlist"/>

    </td>
  </tr>
</table>
</xsl:template>

<xsl:template match="configuration">
      <table class="summary">
        <tr>
          <td class="rt" width="80">Problem : </td>
          <td class="rt_data"><xsl:value-of select="benchidx"/></td>
        </tr>
        <tr>
          <td class="rt">Rand start : </td>
          <td class="rt_data"><xsl:value-of select="pre_runtime"/> ms</td>
        </tr>
        <tr>
          <td class="rt">Runtime : </td>
          <td class="rt_data"><xsl:value-of select="runtime"/> ms</td>
        </tr>
        <tr>
          <td class="rt">Lastchance : </td>
          <td class="rt_data"><xsl:value-of select="post_runtime"/> ms</td>
        </tr>
        <tr>
          <td class="rt">Force-ACK : </td>
          <td class="rt_data"><xsl:value-of select="ack"/></td>
        </tr>
        <tr>
          <td class="rt">Force-Bcast : </td>
          <td class="rt_data"><xsl:value-of select="bcast"/></td>
        </tr>
        <tr>
          <td class="rt">LPL : </td>
          <td class="rt_data"><xsl:value-of select="lpl"/></td>
        </tr>
        <xsl:for-each select="timer">
          <xsl:sort select="@idx"/>
          <tr>
            <td class="rt">Timer(<xsl:value-of select="@idx"/>) : </td>
            <td class="rt_data">
            <xsl:choose>
            <xsl:when test="@oneshot='yes'"> 1sh </xsl:when>
            <xsl:otherwise> per </xsl:otherwise>
            </xsl:choose>
            | <xsl:value-of select="@delay"/>/<xsl:value-of select="@period"/></td>
          </tr>
    
        </xsl:for-each>
      </table>
</xsl:template>

<xsl:template match="stat">
          
          <td class="sub"><xsl:value-of select="REMC"/></td>          
          <td class="rt_data"><xsl:value-of select="TC"/></td>
          <td class="rt_data"><xsl:value-of select="BC"/></td>
          <td class="rt_data"><xsl:value-of select="RC"/></td>

          <td class="rt_data snd"><xsl:value-of select="SC"/></td>
          <td class="rt_data snd"><xsl:value-of select="SSC"/></td>
          <td class="rt_data snd"><xsl:value-of select="SFC"/></td>

          <td class="rt_data snd"><xsl:value-of select="SDC"/></td>
          <td class="rt_data snd"><xsl:value-of select="SDSC"/></td>
          <td class="rt_data snd"><xsl:value-of select="SDFC"/></td>

          <td class="rt_data"><xsl:value-of select="WAC"/></td>
          <td class="rt_data"><xsl:value-of select="NAC"/></td>

          <td class="rt_data rcv"><xsl:value-of select="RCC"/></td>
          <td class="rt_data rcv"><xsl:value-of select="EXC"/></td>
          <td class="rt_data rcv"><xsl:value-of select="WC"/></td>
          <td class="rt_data rcv"><xsl:value-of select="DRC"/></td>
          <td class="rt_data rcv"><xsl:value-of select="FC"/></td>
          <td class="rt_data rcv"><xsl:value-of select="MC"/></td>
          
</xsl:template>

<xsl:template match="debuglist">
  <div class="debuglist">
  <xsl:for-each select="debug">
    <xsl:sort select="@idx"/>
    <strong>Mote <xsl:value-of select="@idx"/> :</strong>
    <xsl:choose>
    <xsl:when test=".='0'">
          <span class="debug_ok">OK</span>
    </xsl:when>
    <xsl:otherwise>
          <span class="debug_fail">FAIL (<xsl:value-of select="."/>)</span>        
    </xsl:otherwise>
    </xsl:choose>
  </xsl:for-each>
  </div>
</xsl:template>

<xsl:template match="error">
  <div class="error"><xsl:value-of select="."/></div>
</xsl:template>

</xsl:stylesheet>
