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

/* Get parameters */

$sect = ( ! isset($HTTP_GET_VARS['section']) ) ? 'sig9' : $HTTP_GET_VARS['section'];
$f    = ( ! isset($HTTP_GET_VARS['f']) ) ? '' : $HTTP_GET_VARS['f'];

/* Connect to MySQL Server */

$DB_LINK = mysql_connect($DB_HOST, $DB_USERNAME, $DB_PASSWORD)
  or die("Could not connect to SQL Server:".mysql_error());
mysql_select_db($DB_NAME)
  or die("Could not select data base $DB_NAME".mysql_error());

/* function = post message */

if( $f == 'post' ) {
  mysql_query(
    "insert into messages(name, email, message, datetime, section, aid) ".
    "  values('$HTTP_POST_VARS[name]',  ".
    "         '$HTTP_POST_VARS[email]', ".
    "         '$HTTP_POST_VARS[msg]',   ".
    "         '".date("y-m-d H:i:s")."',".
    "         '$sect',                  ".
    "          '0')                     ")
    or die(mysql_error());
  header("location: index.php?section=$sect");
}

$qr = mysql_query(
        "SELECT * FROM sections WHERE name='$sect'"
      ) or die(mysql_error());

if( ! $home ) {
  if( mysql_num_rows($qr) == 0) {
      /* Stop buffering */
      ob_end_clean();
      header("location: home.php");
      exit;
  }
}

$r      = mysql_fetch_array($qr);
$parent = $r['parent'];
$title  = $r['title' ];

/* Determine location */

$loc = "<a href='index.php?section=$parent'>$parent</a>".
       " / ".
       "<a href='index.php?section=$sect'>$sect</a>";
$lpar= $parent;

while( $lpar ) {
  $qr1 = mysql_query(
           "SELECT parent,name FROM sections WHERE name='$lpar'"
         ) or die(mysql_error());
  $r1  = mysql_fetch_array($qr1);
  $loc = "<a href='index.php?section=$r1[parent]'>$r1[parent]</a>".
         " / " . $loc;
  $lpar= $r1['parent'];
} 

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


  <!--Section Top Articles-->
  <div class="boxmenu">
   <h5 id="">Top</h5>
   <table>
   <tbody>
   <?PHP

      $s = GetSubsections("$sect") . " sname='$sect' ";
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
 <div id="right_col">
  <?=$loc?>
  <h2><?=$title?> : Articles</h2>
  <div id="contentbox">

  <?PHP if( isset($_GET['aid']) ) {
     /* Show article */
     $qr = mysql_query(
             "SELECT * FROM articles WHERE aid='$aid'"
           ) or die(mysql_error());
           mysql_query(
             "update articles set views=views+1 where aid='$aid'"
           ) or die(mysql_error());
     $r = mysql_fetch_array($qr);
   ?>

   <h3><?=$r[title]?></h3>
   <p>
   <font size="2">
     <i><b>By</b> <?=$r[author]?> <br />
        <b>Viewed</b> <?=$r[views]?> times. <br />
        <b>Description</b> <?=$r[descr]?>
     </i>
   </font>
   </p>

   <p><?=$r[contents]?></p>

   <?PHP } else { ?>

   <table>
   <tbody>

   <?PHP

      $s = GetSubsections("$sect") . " sname='$sect' ";
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
   </tbody>
   </table>

  <?PHP } ?>

  </div>
 </div>

 <div id="bottombar">
 <p align="center">&copy; 2003 Sig9 - <a href="http://creativecommons.org/licenses/by/1.0/"  title="This is a Creative Commons Licence.">Content License</a> - <a  href="http://sig9.com/about/">About</a> - <a href="mailto:webmaster@sig9.com">Webmaster</a>
 </div>
</body>
</html>