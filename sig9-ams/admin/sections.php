<?PHP

/*
 
  sig9-ams:  authors/sections.php
 
  Copyright (C) 2002, 2003 Vivek Mohan <vivek@sig9.com>
  Licensed under the Academic Free License version 1.2
 
  This program is distributed in the hope  that it  will be useful,
  but WITHOUT ANY WARRANTY;  without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  
*/

require './../inc/config.php';

/* Connect to MySQL Server */

$DB_LINK = mysql_connect($DB_HOST, $DB_USERNAME, $DB_PASSWORD)
  or die("Could not connect to SQL Server:".mysql_error());
mysql_select_db($DB_NAME)
  or die("Could not select data base $DB_NAME".mysql_error());

?>

<html>
 <head>
   <title>Article Sections</title>
   <style>
    h1{ font-family: Verdana;}
    td{ font-family: Verdana; font-size: 12px;}
   </style>
 </head>
<body>

<table width="95%" cellspacing="0" align="center">
 <tr>
  <td><img src="./../i/logo.jpg" width="220" /></td>
  <td><h2><b>Sig9 AMS Admin</b></h2></td>
 </tr>
 <tr bgcolor="#89453A" align="center"><td colspan="2"><font color="#ffffff">Sections</font></td>
 </tr>
 <tr><td colspan="2">

  <a href="index.php?action=logout">Logout</a>           |
  <a href="index.php?action=viewarts">My Articles</a>    |
  <a href="index.php?action=newart">Post New Article</a> |
  <a href="sections.php">Manage Article Sections</a>

  <h2>ARTICLE SECTIONS</h2>

  <?PHP

    if( $HTTP_GET_VARS['action'] == 'new' ){
      $qR = mysql_query(
              "SELECT name FROM sections WHERE name='$HTTP_POST_VARS[name]'"
            ) or die(mysql_error());
      if(mysql_num_rows($qR) > 0){
        print "A section with that name already exists.<br/>";
      } else {
        $qR = mysql_query(
                "INSERT INTO sections(name,title,parent)   ".
                "  values( '$HTTP_POST_VARS[name]',        ".
                "          '$HTTP_POST_VARS[title]',       ".
                "          '$HTTP_POST_VARS[sect]')        "
              ) or die(mysql_error());
      }
    }
    elseif( $HTTP_GET_VARS['action'] == 'delete' ){
      mysql_query(
        "DELETE FROM sections WHERE name='$HTTP_GET_VARS[name]'"
      ) or die(mysql_error());
    }
    elseif( $HTTP_GET_VARS['action'] == 'edit' ) {
      $qr = mysql_query(
        "SELECT title FROM sections WHERE name='$HTTP_GET_VARS[name]'"
      ) or die(mysql_error());

  ?>

   <form method="post" action="sections.php?action=save&name=<?=$HTTP_GET_VARS[name]?>">
    <b><h3> Edit Section: <?=$HTTP_GET_VARS[name]?> </h3></b>
    Title<br/>
    <input type=text value="<?=mysql_result($qr,'title')?>" name="title"/><br/>
    <input type=submit value='Save'/>
   </form>
 
  <?PHP

  }
  elseif( $_GET['action'] == 'save' ){
    $qR = mysql_query(
            "UPDATE sections SET title='$HTTP_POST_VARS[title]' ". 
            "  where name='$_GET[name]'"
          ) or die(mysql_error());
  }

  function slist($name, $lvl) {
    $qR = mysql_query("select * from sections where parent='$name'") or die(mysql_error());
    while($r = mysql_fetch_array($qR)) {
      $l = $lvl+4;
      print "\n";
      while( $l-- )
        print " "; 
      print "|____";
      print "<input type=\"radio\" name=\"sect\" value=\"$r[name]\">$r[name] ";
      print "<a href=\"sections.php?action=delete&name=$r[name]\">delete</a> ";
      print "<a href=\"sections.php?action=edit&name=$r[name]\">edit</a> ";
      slist($r[name],$lvl+10);
    }
  }

  ?>


  <div width="100%" style="background-color:lightblue">Section Tree</div>
  <form method="post" action="sections.php?action=new">
   <pre style="font-family:monospace"><input type="radio" name="sect" value="sig9" checked/>SIG9<? slist("sig9",0) ?>
   </pre>

  <div width="100%"><b>Add New Section</b></div>
   <br/>
   Name: <br/>
   <input type="text" name="name"/>
   <input type="hidden" name="action" value="new"/>
   <br/>
   Title: <br/>
   <input type="text" name="title"><br/>
   <input type="Submit" value="Create"><br/>
   <br/>
  </form>

 <br/>

 </td>
 </tr>
 <tr bgcolor="#89453A" align="center"><td colspan="2"><font color="#ffffff">Copyright (c)</font></td>
 </tr>
 </table>
 </body>
</html>
