<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="/">
<html>

<head>
<meta name="generator" content="RadioTest result report" />
<meta name="robots" content="noindex,nofollow" />
<meta http-equiv="content-type" content="text/html; charset=iso-8859-1" />
<meta http-equiv="description" content="RadioTest result report" />
<title>RadioTest result report</title>
<link rel="stylesheet" href="testresults.css" type="text/css"/>
</head>

<body>
<div class="top">Statistics generated at <xsl:value-of select="resultset/@date"/></div>
<xsl:apply-templates/>
</body>
</html>
</xsl:template>

<xsl:template match="testresult">
<table class="testresult">
  <tr valign="top">

    <td width="20%">
      <div class="title">Testcase</div>
      <xsl:apply-templates select="testcase"/>
    </td>

    <td>
      <div class="title">Statistics</div>
      <table class="data">
        <tr>  
          <td class="sub"></td>
          <td class="title_send" colspan="4">&#160;</td>
          <td class="title_send" colspan="3">send()</td>
          <td class="title_send" colspan="3">sendDone()</td>
          <td class="title_send" colspan="2">&#160;</td>
          <td class="title_rcv" colspan="6">receive()</td>
          <td class="sub" colspan="2"><strong>msgID</strong></td>
        </tr>
        <tr>
          <td class="sub"><strong>Edge</strong></td>
          <td class="sub">Rem</td>
          <td class="sub">Triggered</td>
          <td class="sub">Backlog</td>
          <td class="sub">Resend</td>

          <td class="sub">Total</td>
          <td class="sub">SUCCESS</td>
          <td class="sub">FAIL</td>

          <td class="sub">Total</td>
          <td class="sub">SUCCESS</td>
          <td class="sub">FAIL</td>

          <td class="sub">ACK</td>
          <td class="sub">no ACK</td>
 
          <td class="sub">Total</td>
          <td class="sub">Expected</td>
          <td class="sub">Wrong</td>
          <td class="sub">Duplicate</td>
          <td class="sub">Forward</td>
          <td class="sub">Missed</td>

          <td class="sub">Nxt</td>
          <td class="sub">Nxt</td>

        </tr>
        <xsl:for-each select="statlist/stat">
        <tr>
          <xsl:variable name="curr_idx" select="@idx"/>
          <td class="sub"><xsl:value-of select="$curr_idx"/></td>
          <xsl:apply-templates select="current()[@idx=$curr_idx]"/>
          <xsl:apply-templates select="../../finallist/finaledgestate[@idx=$curr_idx]"/>
        </tr>
        </xsl:for-each>
      </table>

<!-- ERROR CHECKING -->
      <div class="title_light">Statistics Error Checking</div>

      <table class="data">
      <tr>
        <td class="sub"><strong>Edge</strong></td>
        <td class="sub">Trigger</td>
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
            <xsl:when test="TC - BC - SC + RC - REMC = 0"><span class="debug_ok">OK</span>
            </xsl:when>
            <xsl:otherwise><span class="debug_fail">ERR</span>
            </xsl:otherwise>
          </xsl:choose>
          </td>

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
            <xsl:when test="../../testcase/ack = 'Off'"><span class="debug_ok">-</span>
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

      <div class="title_light">Mote Internal Error Checking</div>
      <xsl:apply-templates select="debuglist"/>

    </td>
  </tr>
</table>
</xsl:template>

<xsl:template match="testcase">
      <table class="summary">
        <tr>
          <td class="rt" width="80">Problem : </td>
          <td class="rt_data"><xsl:value-of select="idx"/></td>
        </tr>
        <tr>
          <td class="rt">Runtime : </td>
          <td class="rt_data"><xsl:value-of select="runtime"/> ms</td>
        </tr>
        <tr>
          <td class="rt">Trigger : </td>
          <td class="rt_data"><xsl:value-of select="trigger"/> ms</td>
        </tr>
        <tr>
          <td class="rt">ACK : </td>
          <td class="rt_data"><xsl:value-of select="ack"/></td>
        </tr>
        <tr>
          <td class="rt">DirectAddr : </td>
          <td class="rt_data"><xsl:value-of select="daddr"/></td>
        </tr>
        <tr>
          <td class="rt">LPL : </td>
          <td class="rt_data"><xsl:value-of select="lpl"/> (<xsl:value-of select="lplintval"/> ms)</td>
        </tr>
        <tr>
          <td class="rt">LastChance : </td>
          <td class="rt_data"><xsl:value-of select="lastchance"/> ms</td>
        </tr>
      </table>
</xsl:template>

<xsl:template match="stat">
          
          <td class="sub"><xsl:value-of select="REMC"/></td>          
          <td class="rt_data snd"><xsl:value-of select="TC"/></td>
          <td class="rt_data snd"><xsl:value-of select="BC"/></td>

          <td class="rt_data snd"><xsl:value-of select="RC"/></td>

          <td class="rt_data snd"><xsl:value-of select="SC"/></td>
          <td class="rt_data snd"><xsl:value-of select="SSC"/></td>
          <td class="rt_data snd"><xsl:value-of select="SFC"/></td>

          <td class="rt_data snd"><xsl:value-of select="SDC"/></td>
          <td class="rt_data snd"><xsl:value-of select="SDSC"/></td>
          <td class="rt_data snd"><xsl:value-of select="SDFC"/></td>

          <td class="rt_data snd"><xsl:value-of select="WAC"/></td>
          <td class="rt_data snd"><xsl:value-of select="NAC"/></td>

          <td class="rt_data rcv"><xsl:value-of select="RCC"/></td>
          <td class="rt_data rcv"><xsl:value-of select="EXC"/></td>
          <td class="rt_data rcv"><xsl:value-of select="WC"/></td>
          <td class="rt_data rcv"><xsl:value-of select="DRC"/></td>
          <td class="rt_data rcv"><xsl:value-of select="FC"/></td>
          <td class="rt_data rcv"><xsl:value-of select="MC"/></td>
          
</xsl:template>

<xsl:template match="finaledgestate">
          <td class="rt_data snd"><xsl:value-of select="SNM"/></td>
          <td class="rt_data rcv"><xsl:value-of select="RNM"/></td>
</xsl:template>

<xsl:template match="debuglist">
  <div>
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

</xsl:stylesheet>
