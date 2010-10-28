<?PHP

/*
 
  sig9-ams:  install.php - script installer
 
  Copyright (C) 2002, 2003 Vivek Mohan <vivek@sig9.com>
  Licensed under the Academic Free License version 1.2
 
  This program is distributed in the hope  that it  will be useful,
  but WITHOUT ANY WARRANTY;  without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  
*/

?>

<html>
<head>
 <title>Script Installer</title> 
</head>
<body> 
 
<?PHP

if($HTTP_GET_VARS['action'] == 'install'){

  print '<p>';
  print "<b>Creating config file...</b>";
 
  /* Write config.php */

  $fh = fopen("./inc/config.php", "w");
  fwrite($fh, "<?

  \$DB_NAME        = \"$HTTP_POST_VARS[dbname]\"	;
  \$DB_USERNAME	   = \"$HTTP_POST_VARS[dbusername]\"	;
  \$DB_PASSWORD	   = \"$HTTP_POST_VARS[dbpassword]\"	;
  \$DB_HOST	   = \"$HTTP_POST_VARS[dbhost]\"	;

  ?>" );

  fclose($fh);
  print 'Done.<br />';

  require('inc/config.php');

  print '<b>Creating Tables...</b>';
  print '<blockquote>';

  /* mysql server connect and select db */
	
  mysql_connect($DB_HOST, $DB_USERNAME, $DB_PASSWORD) 
    or die(mysql_error());
  mysql_select_db($DB_NAME) 
    or die(mysql_error());

  /* drop tables if already existent */

  mysql_query(
    "DROP TABLE IF EXISTS authors, articles, sections"
  ) or die(mysql_error());

  /* Create tables */

  mysql_query(
    'CREATE TABLE articles (             '.
    '  aid        INT         NOT NULL AUTO_INCREMENT, '.
    '  title      TINYTEXT    NOT NULL , '.
    '  descr      TINYTEXT    NOT NULL , '.
    '  contents   TEXT        NOT NULL , '.
    '  keywords   TINYTEXT    NOT NULL , '.
    '  author     VARCHAR(20) NOT NULL , '.
    '  sname      VARCHAR(50) NOT NULL , '.
    '  timestamp  DATETIME    NOT NULL , '.
    '  views      INT         NOT NULL , '.
    '  status     TINYINT     NOT NULL , '.
    '  PRIMARY KEY(aid))                 '
  ) or die('Failed to create table <i>articles</i>'. mysql_error());

  mysql_query(
    '  CREATE TABLE sections (           '.
    '    parent   VARCHAR(20) NOT NULL , '.
    '    name     VARCHAR(20) NOT NULL , '.
    '    title    TINYTEXT    NOT NULL ) '
  ) or die(mysql_error());

  print '</blockquote>';
  print '<b>Done.</b>';
  print '<br /><b>Script installed successfully.</b>';

  exit;
}

?>

<h1>Sig9-AMS Script Installer</h1>
<hr />

<form name="install" action="install.php?action=install" method="post">
 <font face=verdana size=1>
  MySQL Database Name<br />
  <input type="text" name="dbname" value="" size="25" /><br/>
  MySQL User Name<br />
  <input type="text" name="dbusername" value="" size="25" /><br />
  MySQL Password<br />
  <input type="text" name="dbpassword" value="" size="25" /><br />
  MySQL Host<br />
  <input type="text" name="dbhost" value="localhost" size="25" /><br />
  Script Administrator Id<br />
  <input type="text" name="adminid" value="admin" size="25" /><br />
  Script Administrator Password<br />
  <input type="password" name="adminpass" size=25 /><br /><br />
  <input type="submit" value="INSTALL" />
 </font>
</form>

<hr />

</body>
</html>