<?PHP

/*
 
  sig9-ams:  index.php
 
  Copyright (C) 2002, 2003 Vivek Mohan <vivek@sig9.com>
  Licensed under the Academic Free License version 1.2
 
  This program is distributed in the hope  that it  will be useful,
  but WITHOUT ANY WARRANTY;  without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  
*/

require 'inc/config.php';

/* Connect to MySQL Server */

$DB_LINK = mysql_connect($DB_HOST, $DB_USERNAME, $DB_PASSWORD)
  or die("Could not connect to SQL Server:".mysql_error());
mysql_select_db($DB_NAME)
  or die("Could not select data base $DB_NAME".mysql_error());


function GetSubsections($section){
  $subs = "";
  $qr = mysql_query("SELECT name FROM sections WHERE parent='$section'")
          or die(mysql_error());
  while( $r = mysql_fetch_array($qr)){
    $subs .= "sname='".$r['name'] . "' OR ";
    $subs .= GetSubsections($r['name']);
  }
  return $subs;  
}

?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
   "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html>
  <head>
    <meta http-equiv="content-type" content="text/html; charset=UTF-8" />
    <meta name="keywords" content="sig9, articles, unix, bsd, operating systems, C, C++" />
    <meta name="description" content="SiG9 documentation project" />
    <meta name="robots" content="all" />
    <link rel="stylesheet" type="text/css" href="./../theme/style.css" />
    <link rel="stylesheet" type="text/css" href="style.css" />
    <title>Special Interest Group 9 :: <?=$title?></title>
  </head>
<body>

 <!-- TOP BAR -->
 <div id="topbar">
  <a href="http://sig9.com/">special interest group 9</a> /
  <a href="http://sig9.com/articles/">articles</a> /
 </div>
 <!-- / TOP BAR -->

 <!-- HEADING -->
 <div id="headbar"><h1>Articles</h1></div>
 <!-- / HEADING -->

 <div id="content">
 <div id="left_col">

  <div id="navbarmenu">
    <img src="./i/folder.gif" alt=""/>
     <a href="index.php?section=<?=$sect?>">Sig9</a>
  </div>

  <div id="navbarsubmenu">
   <table width="100%">
   <tbody>
   <?PHP

     $qr = mysql_query(
             "SELECT name,title FROM sections WHERE parent='sig9'"
           ) or die(mysql_error());
     while( $r = mysql_fetch_array($qr) ) {
       print "<tr><td valign=\"top\" width=\"10%\"><img alt=\"\" src=\"./i/folder.gif\" /></td>";
       print "<td><a href=\"index.php?section=$r[name]\">";
       print "$r[title]</a></td></tr>";
     }

   ?>
   </tbody>
   </table>
  </div>	

  <!--Section Top Articles-->
  <div class="boxmenu">
   <h5 id="">Top</h5>
   <table>
   <tbody>
   <?PHP

      $s = GetSubsections("sig9") . " sname='null' ";
      $qr = mysql_query(
              "SELECT aid,title,views,author,sname FROM articles WHERE $s ".
              "    order by views desc limit 0,10 "
            ) or die(mysql_error());
      while( $r = mysql_fetch_array($qr) ) {
        print "<tr><td valign='top'><img alt=\"\" src=\"./i/file.gif\" /></td><td>";
        print "<a href=\"index.php?section=$r[sname]&amp;aid=$r[aid]\">";
        print "$r[title]</a> ($r[views])</td></tr>";
      }

    ?>
   </tbody>
   </table>
  </div>
 </div>
 <!--/left col-->

 <div id="right_col">
  <div id="contentbox">

  <p>Welcome ! If you'd like to submit an article, please mail it to us
  at <a href="mailto:submissions@sig9.com">submissions@sig9.com</a>.You 
  can use the <a href="http://sig9.com/forum">forum</a> for discussions 
  regarding the articles posted in here.</p>

  <?PHP 	

      $s = GetSubsections("sig9") . " sname='null' ";
      $qr = mysql_query(
              "SELECT * FROM articles WHERE $s ORDER BY ".
              "  timestamp desc limit 0,10"
            ) or die(mysql_error());
      while ($r = mysql_fetch_array($qr)) {	
        print "<div class=\"art_list\"> \n";
        print " <h4>$r[title]</h4> \n";
	print " <div class=\"art_list_author_text\"> \n".
              "  Posted on  $r[timestamp]  by  $r[author] \n".
              "  | Viewed $r[views]  times \n".
	      " </div> \n";
	print " <div class=\"art_list_desc\">$r[descr]</div> \n";
	print " <span class='art_list_read'> \n".
	      "   <a href=\"index.php?section=$r[sname]&amp;aid=$r[aid]\">Read</a> \n".
	      " </span> \n";
	print "</div> \n";
      }
  ?>
  </div>
 </div>  
 </div>

 <div id="bottombar">
  <p align="center">&copy; 2003 Sig9 - 
   <a href="http://creativecommons.org/licenses/by/1.0/"  title="This is a Creative Commons 
   Licence.">Content License</a> - 
   <a  href="http://sig9.com/about/">About</a> - 
   <a href="mailto:webmaster@sig9.com">Webmaster</a>
  </p>
 </div>

</body>
</html>
