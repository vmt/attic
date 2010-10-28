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

/* Get parameters */

$sect = ( ! isset($HTTP_GET_VARS['section']) ) ? 'sig9' : $HTTP_GET_VARS['section']; 
$f    = ( ! isset($HTTP_GET_VARS['f']) ) ? '' : $HTTP_GET_VARS['f'];

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
  if( mysql_num_rows($qr) == 0)
	header("location: home.php");
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

?>

<!DOCTYPE PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" 
	"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html>
  <head>
    <meta http-equiv="content-type" content="text/html; charset=UTF-8" />
    <meta name="keywords" content="sig9, articles, unix, bsd, operating systems, C, C++" />
    <meta name="description" content="SiG9 documentation project" />
    <meta name="robots" content="all" />
    <link rel="stylesheet" type="text/css" href="style.css" />
    <title>Special Interest Group 9 :: <?=$title?></title>
  </head>
<body>

<!-- HEADER -->

<table border="0" width="100%" cellpadding="3" align="center">
 <tbody>
  <tr>
   <td width="30%">
    <table border="0" cellspacing="0" width="100%">
     <tr>
      <td width="100%"><img border="0" src="./i/logo.jpg"></td>
     </tr>
    </table>
   </td>
   <td width="70%">
    <p align="center">
     <br />
     <font size="2" face="verdana">
      <b>S</b>pecial
      <b>I</b>nterest <b>G</b>roup <b>9</b>
     </font>
     <br />
     <font face="verdana" size="1" align=center><?=$loc?></font>
    </p>
   </td>
  </tr>

 </tbody>
</table>

<!-- END OF HEADER -->
