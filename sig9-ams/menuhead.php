<?PHP

/*
 
  sig9-ams:  menuhead.php
 
  Copyright (C) 2002, 2003 Vivek Mohan <vivek@sig9.com>
  Licensed under the Academic Free License version 1.2
 
  This program is distributed in the hope  that it  will be useful,
  but WITHOUT ANY WARRANTY;  without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  
*/

?>

 <div id="left_col">

   <div id="navbarmenu">
      <img src="./i/back.gif"   /> 
      <a href="index.php?section=<?=$parent?>">Up</a><br />
      <img src="./i/folder.gif" />
      <a href="index.php?section=<?=$sect?>"><?=$title?></a>
   </div>

   <div id="navbarsubmenu">
   <table width="100%">
   <?PHP
     $qr = mysql_query(
             "SELECT name,title FROM sections WHERE parent='$sect'"
           ) or die(mysql_error());
     while( $r = mysql_fetch_array($qr) ) {
       print "<tr><td valign=\"top\"><img src=\"./i/folder.gif\" /></td>";
       print "<td><a href=\"index.php?section=$r[name]\">";
       print "$r[title]</a></td></tr>";
     }
   ?>
   </table>
   </div>	

   <h5 style="border: 1px solid #efefef"><?=$title?>: Top</h5>
   <div class="boxmenu">
   <table>
   <?PHP

      $s = GetSubsections("$sect") . " sname='null' ";
      $qr = mysql_query(
              "SELECT aid,title,views,author,sname FROM articles WHERE $s ".
              "    order by views desc limit 0,10 "
            ) or die(mysql_error());
      while( $r = mysql_fetch_array($qr) ) {
        print "<tr><td valign='top'><img src=\"./i/file.gif\" /></td><td>";
        print "<a href=\"index.php?section=$r[sname]&aid=$r[aid]\">";
        print "$r[title]</a> ($r[views]) by <i>$r[author]</i></td></tr>";
      }
    ?>
   </table>
   </div>

   <h5 style="border: 1px solid #efefef"><?=$title?>: Latest</h5>
   <div class="boxmenu">
   <table>
   <?PHP
      $s = GetSubsections("$sect") . " sname='null' ";
      $qr = mysql_query(
              "SELECT aid,title,views,author,sname FROM articles WHERE $s ".
              "    order by timestamp desc limit 0,10 "
            ) or die(mysql_error());
      while( $r = mysql_fetch_array($qr) ) {
        print "<tr><td valign='top'><img src=\"./i/file.gif\" /></td><td>";
        print "<a href=\"index.php?section=$r[sname]&aid=$r[aid]\">";
        print "$r[title]</a> ($r[views]) by <i>$r[author]</i></td></tr>";
      }
    ?>
    </table>
    </div>
 </div>

<table border="0" width="100%" cellpadding="3" align="center">
<tr>
<td width="30%" valign=top id="sidebar">

<!------------------------- MENU COLUMN --------------------------->


   <h2>Nav Bar</h2>
   <div id="navbarmenu">
      <img src="./i/back.gif"   /> 
      <a href="index.php?section=<?=$parent?>">Up</a><br />
      <img src="./i/folder.gif" />
      <a href="index.php?section=<?=$sect?>"><?=$title?></a>
    </div>
    <div id="navbarsubmenu">

    <?PHP

      $qr = mysql_query(
              "SELECT name,title FROM sections WHERE parent='$sect'"
            ) or die(mysql_error());

      while( $r = mysql_fetch_array($qr) ) {
        print "<img src=\"./i/folder.gif\" /> ";
        print "<a href=\"index.php?section=$r[name]\">";
        print "$r[title]</a><br/>";
      }

/*
      $qr = mysql_query(
              "SELECT name,title FROM sections WHERE parent='$parent' ".
              "   and strcmp(name, '$sect') "
            ) or die(mysql_error());

      while( $r = mysql_fetch_array($qr) ) {
        print "<img src=\"./i/folder.gif\"> ";
        print "<a href=\"index.php?section=$r[name]\">";
        print "$r[title]</a><br />";
      }

*/

    ?>

    </div>	
  </div>

  <br />


    <h3><?=$title?>: Top</h3>
    <div class="boxmenu">

    <table>

    <?PHP

      $s = GetSubsections($sect) . " sname='$sect' ";

      $qr = mysql_query(
              "SELECT aid,title,views,author,sname FROM articles WHERE $s ".
              "    order by views desc limit 0,10 "
            ) or die(mysql_error());
      while( $r = mysql_fetch_array($qr) ) {
        print "<tr>";
        print "<td valign='top'><img src=\"./i/file.gif\" /></td><td>";
        print "<a href=\"index.php?section=$r[sname]&aid=$r[aid]\">";
        print "$r[title]</a> ($r[views]) by <i>$r[author]</i></td>";
        print "</tr>";
      }

    ?>

    </table>
    </div>


  <br />

    <h3><?=$title?>: Latest</h3>

    <div class="boxmenu">
    <table>

    <?PHP

      $s = GetSubsections($sect) . " sname='$sect' ";

      $qr = mysql_query(
              "SELECT aid,title,views,author,sname FROM articles WHERE $s ".
              "    order by timestamp desc limit 0,10 "
            ) or die(mysql_error());
      while( $r = mysql_fetch_array($qr) ) {
        print "<tr>";
        print "<td valign='top'><img src=\"./i/file.gif\" /></td>";
        print "<td><a href=\"index.php?section=$r[sname]&aid=$r[aid]\">";
        print "$r[title]</a> ($r[views]) by <i>$r[author]</i></td>";
        print "</tr>";
      }

    ?>

    </table>
    </div>

  <br />

</td>
<td width="70%" valign=top id="contentbar">
