<?PHP

/*
 
  sig9-ams:  authors/index.php
 
  Copyright (C) 2002, 2003 Vivek Mohan <vivek@sig9.com>
  Licensed under the Academic Free License version 1.2
 
  This program is distributed in the hope  that it  will be useful,
  but WITHOUT ANY WARRANTY;  without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  
*/

/*


************** ADD YOUR OWN AUTHORIZATION CODE HERE ****************

*/

require './../inc/config.php';

/* Connect to MySQL Server */

$DB_LINK = mysql_connect($DB_HOST, $DB_USERNAME, $DB_PASSWORD)
  or die("Could not connect to SQL Server:".mysql_error());
mysql_select_db($DB_NAME)
  or die("Could not select data base $DB_NAME".mysql_error());

/* logout */

if( $HTTP_GET_VARS['action'] == 'logout' ) {
  unset($_SERVER['PHP_AUTH_USER']);
  header("location: ../index.php");
  exit;
}

/* Post New article */

if( $HTTP_GET_VARS['action'] == 'post' ) {
  $dt = date("y-m-d H:i:s");

//  print $HTTP_POST_VARS[contents];
//  $HTTP_POST_VARS[contents] = str_replace("'","\'", $HTTP_POST_VARS[contents]);

  $HTTP_POST_VARS[contents] = strip_tags( $HTTP_POST_VARS[contents],
		"<a><b><i><ul><ol><li><p><br><pre>".
                "<blockquote><font><h1><h2><h3><h4><h5><h6>" );
  mysql_query(
    "INSERT INTO articles(title, descr, keywords, author, sname, timestamp, contents) ".
    "  values( '$HTTP_POST_VARS[title]',     ".
    "          '$HTTP_POST_VARS[descr]',     ".
    "          '$HTTP_POST_VARS[keywords]',  ".
    "          '$HTTP_POST_VARS[author]',    ".
    "          '$HTTP_POST_VARS[sname]',     ".
    "          '$dt' ,                       ".
    "          '$HTTP_POST_VARS[contents]')  "
  ) or die(mysql_error());

  header("location: index.php");
}

/* Delete Article */

if( $HTTP_GET_VARS['action'] == 'delete' ) {
  mysql_query(
    "DELETE FROM articles WHERE aid='$HTTP_GET_VARS[aid]'"
  ) or die(mysql_error());

  header("location: index.php");
}

/* Edit Article */

if( $HTTP_GET_VARS['action'] == 'edit' ) {

  $qr = mysql_query(
          "SELECT * FROM articles WHERE aid='$HTTP_GET_VARS[aid]'"
        ) or die(mysql_error());
  $r  = mysql_fetch_array($qr);

  $HTTP_POST_VARS['title']    = $r["title"];
  $HTTP_POST_VARS['keywords'] = $r["keywords"];
  $HTTP_POST_VARS['descr']    = $r["descr"];
  $HTTP_POST_VARS['sname']    = $r["sname"];
  $HTTP_POST_VARS['contents'] = $r["contents"];
  $HTTP_POST_VARS['author']   = $r["author"];

}

/*  Save article */

if( $HTTP_GET_VARS['action'] == 'save' ) {

//  $HTTP_POST_VARS[contents] = str_replace("'","\'", $HTTP_POST_VARS[contents]);
  $HTTP_POST_VARS[contents] = strip_tags( $HTTP_POST_VARS[contents],
		"<a><b><i><ul><ol><li><p><br><pre>".
                "<blockquote><font><h1><h2><h3><h4><h5><h6>" );

  $qr = mysql_query(
          "UPDATE articles SET ".
          "  author='$HTTP_POST_VARS[author]',     ".
          "  title='$HTTP_POST_VARS[title]',       ".
          "  keywords='$HTTP_POST_VARS[keywords]', ".
          "  descr='$HTTP_POST_VARS[descr]',       ".
          "  sname='$HTTP_POST_VARS[sname]',       ".
          "  contents='$HTTP_POST_VARS[contents]'  ".
          "  where aid='$HTTP_GET_VARS[aid]'       "
        ) or die(mysql_error());
}

/* section list generator */

function slist($name, $lvl, $sl) {
  $qR = mysql_query(
          "SELECT * FROM sections WHERE parent='$name'"
        ) or die(mysql_error());
  while($r = mysql_fetch_array($qR)) {
    if( $sl == $r[name] )
      print "<option value='$r[name]' selected>";
    else
      print "<option value='$r[name]'>";
    $l = $lvl+4;
    while( $l-- )  print "&nbsp;"; 
    print "|____";
    print "$r[name]";
    print "</option>";
    slist($r[name],$lvl+10,$sl);
  }
}

?>

<html>
 <head>
   <title>Admin</title>
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
 <tr bgcolor="#89453A" align="center"><td colspan="2"><font color="#ffffff">Admin Cpanel</font></td>
 </tr>
 <tr><td colspan="2">

  <a href="index.php?action=logout">Logout</a>    |
  <a href="index.php?action=viewarts">My Articles</a>    |
  <a href="index.php?action=newart">Post New Article</a> |
  <a href="sections.php">Manage Article Sections</a
  <br/>

 <?PHP
   if( $HTTP_GET_VARS['action'] == 'newart' ){ 
 ?>

 <h2>Post New Article</h2>

 <form method="POST" action="index.php?action=post">
  <p>Author<br /><input type="text" name="author" size="20" /></p>
  <p>Title<br /><input type="text" name="title" size="20" /></p>
  <p>Keywords<br /><input type="text" name="keywords" size="20" /></p>
  <p>Description<br />
     <textarea rows="3" name="descr" cols="65"></textarea>
  </p>
  <p>Contents<br />
     <textarea rows="20" name="contents" cols="65"></textarea>
  </p>
  <p><input type="submit" value="Post" /> Into Section 
     <select name="sname">
      <option>SIG9</option>
      <?PHP slist("sig9",0,$HTTP_POST_VARS[sname]); ?>
     </select>
  </p>
 </form>


 <?PHP

   } 
   if( $HTTP_GET_VARS['action'] == 'viewarts' || ! $HTTP_GET_VARS['action'] ) { 

     print "<h2>ARTICLES</h2>";
     $qr = mysql_query(
             "SELECT aid, title, sname, author FROM articles"
           ) or die(mysql_error());
     print "<table cellspacing=\"1\" border=\"0\" width=\"100%\">";
     print "<tr>";
     print "<td bgcolor='lightblue'><b>Title</b></td>"   .
           "<td bgcolor='lightblue'><b>Section</b></td>" .
           "<td bgcolor='lightblue'><b>Delete</b></td>"  .
           "<td bgcolor='lightblue'><b>Edit</b></td>"    .
           "<td bgcolor='lightblue'><b>Author</b></td>"  ;
     print "</tr>";
     while( $r = mysql_fetch_array($qr)) {
       print "<tr>";
       print "<td>$r[title]</td>";
       print "<td>$r[sname]</td>";
       print "<td><a href=\"index.php?action=delete&aid=$r[aid]\">delete</a></td>";
       print "<td><a href=\"index.php?action=edit&aid=$r[aid]\">edit</a></td>";
       print "<td>$r[author]</td>";
       print "</tr>";
     }
     print "</table>";
  }
  if( $HTTP_GET_VARS['action'] == 'edit' || $HTTP_GET_VARS['action'] == 'save' ){ 
 ?>

 <h2>EDIT ARTICLE</h2>
 <form method="POST" action="index.php?action=save&aid=<?=$HTTP_GET_VARS[aid]?>">
  <p>Author<br />
     <input type="text" name="author" value="<?=$HTTP_POST_VARS[author]?>" size="20" />
  </p>
  <p>Title<br />
     <input type="text" name="title" value="<?=$HTTP_POST_VARS[title]?>" size="20" />
  </p>
  <p>Keywords<br />
     <input type="text" name="keywords" value="<?=$HTTP_POST_VARS[keywords]?>" size="20" />
  </p>
  <p>Description<br />
     <textarea rows="3" name="descr" cols="55"><?=$HTTP_POST_VARS[descr]?></textarea>
  </p>
  <p>Contents<br />
      <small>
       <b>Allowed Tags :</b><i>
        &lt;a&gt; &lt;b&gt; &lt;i&gt;
        &lt;pre&gt;  &lt;ul&gt;  &lt;ol&gt;
        &lt;li&gt; &lt;br&gt;  &lt;blockquote&gt; <br />
        &lt;font&gt; &lt;h1&gt; &lt;h2&gt;
        &lt;h3&gt; &lt;h4&gt;  &lt;h5&gt;
        &lt;h6&gt; <br />
     <textarea rows="17" name="contents" cols="55"><?=$HTTP_POST_VARS[contents]?></textarea>
  </p>
  <p>
     <input type="submit" value="Save" /> Into Section 
     <select name="sname">
      <option>SIG9</option>
      <?PHP slist("sig9",0,$HTTP_POST_VARS[sname]); ?>
     </select>
  </p>
 </form>

 <?PHP 
   }   
   if( $HTTP_GET_VARS['action'] == 'chpass' ||
       ($HTTP_GET_VARS['action'] == 'savepass' && $estat )){ 
 ?>

 <font color="red"><b><?PHP echo $estat;?></b></font>

 <h2>CHANGE PASSWORD</h2>
 <form method="POST" action="index.php?action=savepass">
  <p>New Password<br />
     <input type="password" name="password1" />
  </p>
  <p>Confirm Password<br />
     <input type="password" name="password2" />
  </p>
  <input type="submit" value="Save" />
 </form>

 <?PHP
   }
 ?>

 <br/>
 </td>
  </tr>
 <tr bgcolor="#89453A" align="center"><td colspan="2"><font color="#ffffff">Copyright (c)</font></td>
 </tr>
</table>
 </body>
</html>
