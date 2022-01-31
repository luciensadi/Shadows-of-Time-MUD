<?php
 global $_GET, $_POST, $view;
 echo "<html>
 <head>
  <title>Help File Editor</title>
 </head>
<body>";

$connect = mysql_connect("localhost", "aod_fotn", "uqxtwege")
or die ("Could not connect to the database");

mysql_select_db("aod_fotn", $connect)
or die ("Could not select the database");

if($_GET['view'] == "")
{
 // Start default view
 // mysql -h sql.genesismuds.com -u dsmud -p
 /* Some Notes:
 $story = mysql_fetch_array($story_result);
 $story_sql = "select * from stories wehre...blahblahbh"
 $story_result = mysql_query($story_sql, $conn);
 */

 if(@$_GET['action'] == "do_delete")
 {
  if($_GET['help_id'] > 0)
  {
   $delete_help = mysql_query("DELETE FROM helps WHERE ID = '".$_GET['help_id']."'");
  }
 }
 $select_help = mysql_query("select * from helps order by ID");
 $num_help = mysql_num_rows($select_help);

 echo "
 <h1>Dragonsworn Help File - Index</h1>
  <a href='$PHP_SELF?view=add_help' style='border: 1px solid darkblue;'>Add Help File</A><br />
  <table width=100% cellpadding=5 cellspacing=1 style='border: 1px solid #000000;'>
   <tr>
    <td style='border: 1px solid #000000;'><b>ID</b></td>
    <td style='border: 1px solid #000000;'><b>Name</b></td>
    <td style='border: 1px solid #000000;'><b>Keywords</b></td>
    <td style='border: 1px solid #000000;'><b>Modify</b></td></tr>";

    while($fetch_help = mysql_fetch_array($select_help))
    {
     echo "<tr>
      <td style='border: 1px solid #EFEFEF;'>".$fetch_help['ID']."</td>
      <td style='border: 1px solid #EFEFEF;'>".$fetch_help['name']."</td>
      <td style='border: 1px solid #EFEFEF;'>".$fetch_help['keyword']."</td>
      <td style='border: 1px solid #EFEFEF;'>[<A HREF='$PHP_SELF?view=edit_help&help_id=".$fetch_help['ID']."'>EDIT</A>] [<A HREF='$PHP_SELF?view=delete_help&help_id=".$fetch_help['ID']."'>DELETE</A>]</td>
     </tr>";
    }

    echo "</table>";
    mysql_free_result($select_help);
 // End default view
}elseif($_GET['view'] == "edit_help" || $_POST['view'] == "edit_help")
{
 // Start view "edit_help"
 echo "<h1>Dragonsworn Help File - Edit Help</h1>
  <a href='$PHP_SELF?view=' style='border: 1px solid darkblue;'>Help File Index</A><br />";

   if($_POST['help_id'] != "")
   {
    $help_id = $_POST['help_id'];
   }else
   {
    $help_id = $_GET['help_id'];
   }
 if($help_id > 0 )
 {
  if($_POST['action'] == 'update_help')
  {
  /*
   if($_POST['update_help_level'] > 0)
   {
    if($_POST['update_help_name'] != '')
    {
     if($_POST['update_help_keyword'] != '')
     {
      if($_POST['update_help_related'] != '')
      {
       if($_POST['update_help_catID'] > 0)
       {
        if($_POST['update_help_body'] != '')
        {*/
         $update_help = mysql_query("UPDATE helps SET level = '".$_POST['update_help_level']."', keyword = '".$_POST['update_help_keyword']."', related = '".$_POST['update_help_related']."', catID = '".$_POST['update_help_catID']."', body = '".$_POST['update_help_body']."', name = '".$_POST['update_help_name']."' WHERE ID = '".$_POST['help_id']."'");
         $update_help = "true";
        /*}else{$update_help = "invalid_body";}
       }else{$update_help = "invalid_catID";}
      }else{$update_help = "invalid_related";}
     }else{$update_help = "invalid_keyword";}
    }else{$update_help = "invalid_name";}
   }else{$update_help = "invalid_level";}*/
  }
  $fetch_help = mysql_fetch_array(mysql_query("SELECT * FROM helps WHERE ID = '$help_id' LIMIT 1"));


  if($update_help != '')
  {
   
   echo "
   <div align='center'>
    <br /><br />";
   if($update_help != '')
   {
    if($update_help == 'true')
    {
     echo "Help file updated successfully!";
    }elseif($update_help == 'invalid_body')
    {
     echo "Error: Please enter a valid body!";
    }elseif($update_help == 'invalid_catID')
    {
     echo "Error: Please enter a catID!";
    }elseif($update_help == 'invalid_name')
    {
     echo "Error: Please enter a name!";
    }elseif($update_help == 'invalid_level')
    {
     echo "Error: Please enter a level!";
    }elseif($update_help == 'invalid_related')
    {
     echo "Error: Please enter a valid related!";
    }elseif($update_help == 'invalid_keyword')
    {
     echo "Error: Please enter a valid keyword!";
    }else
    {
     echo "Error: $update_help";
    }
   }
   echo "
    <br /><br />
   </div>";
  }

  echo "
  <table width=100% cellpadding=5 cellspacing=1 style='border: 1px solid #000000;'>
   <form method='post' action='$PHP_SELF' style='padding: 0px 0px 0px 0px; margin: 0px 0px 0px 0px;'>
   <tr>
    <td style='border: 1px solid #000000;'><b>ID</b></td>
    <td style='border: 1px solid #000000;'><b>".$fetch_help['ID']."</b></td>
   </tr>
   <tr>
    <td style='border: 1px solid #000000;'><b>Level</b></td>
    <td style='border: 1px solid #000000;'><input type='text' size='30' name='update_help_level' value='";
    if($_POST['update_help_level'] != '' && $update_help != 'true')
    {
     echo $_POST['update_help_level'];
    }else
    {
     echo $fetch_help['level'];
    }
    echo "'></td>
   </tr>
   <tr>
    <td style='border: 1px solid #000000;'><b>catID</b></td>
    <td style='border: 1px solid #000000;'><input type='text'  size='30' name='update_help_catID' value='";
    if($_POST['update_help_catID'] != '' && $update_help != 'true')
    {
     echo $$_POST['update_help_catID'];
    }else
    {
     echo $fetch_help['catID'];
    }
    echo "'></td>
   </tr>
   <tr>
    <td style='border: 1px solid #000000;'><b>Name</b></td>
    <td style='border: 1px solid #000000;'><input type='text'  size='30' name='update_help_name' value='";
    if($_POST['update_help_name'] != '' && $update_help != 'true')
    {
     echo $_POST['update_help_name'];
    }else
    {
     echo $fetch_help['name'];
    }
    echo "'></td>
   </tr>
   <tr>
    <td style='border: 1px solid #000000;'><b>Keywords</b></td>
    <td style='border: 1px solid #000000;'><textarea rows='7' cols='45' name='update_help_keyword'>";
    if($_POST['update_help_keyword'] != '' && $update_help != 'true')
    {
     echo stripslashes($_POST['update_help_keyword']);
    }else
    {
     echo stripslashes($fetch_help['keyword']);
    }
    echo "</textarea></td>
   </tr>
   <tr>
    <td style='border: 1px solid #000000;'><b>Related</b></td>
    <td style='border: 1px solid #000000;'><textarea rows='7' cols='45' name='update_help_related'>";
    if($_POST['update_help_related'] != '' && $update_help != 'true')
    {
     echo stripslashes($_POST['update_help_related']);
    }else
    {
     echo stripslashes($fetch_help['related']);
    }
    echo "</textarea></td>
   </tr>
   <tr>
    <td style='border: 1px solid #000000;'><b>Body</b></td>
    <td style='border: 1px solid #000000;'><textarea rows='30' cols='90' name='update_help_body' rows='5' cols='45'>";
    if($_POST['update_help_body'] != '' && $update_help != 'true')
    {
     echo stripslashes($_POST['update_help_body']);
    }else
    {
     echo stripslashes($fetch_help['body']);
    }
    echo "</textarea></td>
   </tr>
  </table>
  <div align='center'>
   <br /><br />";
   
   if($_POST['view'] != "")
   {
    $view = $_POST['view'];
   }else
   {
    $view = $_GET['view'];
   }

   if($_POST['help_id'] != "")
   {
    $help_id = $_POST['help_id'];
   }else
   {
    $help_id = $_GET['help_id'];
   }
   
   echo "
   <input type='hidden' name='action' value='update_help'><input type='hidden' name='view' value='$view'><input type='hidden' name='help_id' value='$help_id'>
   <input type='submit' value='Update Help File'> <input type='reset' value='Reset Form'>
  </div>
  </form>";
 }else
 {
  echo "
  <div align='center'>
   <br /><br />
   Error: No help file specified. <a href='javascript: history.go(-1);'>Please go back</a>.
  </div>";
 }
 // End view "edit_help"
}elseif($_GET['view'] == "delete_help" || $_POST['view'] == "delete_help")
{
 // Start view "delete_help"
 if($_POST['help_id'] != "")
 {
  $help_id = $_POST['help_id'];
 }else
 {
  $help_id = $_GET['help_id'];
 }
   
 echo "<h1>Dragonsworn Help File - Delete Help</h1>
  <a href='$PHP_SELF?view=' style='border: 1px solid darkblue;'>Help File Index</A><br />";
 if($help_id > 0)
 {
  $fetch_help = mysql_fetch_array(mysql_query("SELECT * FROM helps WHERE ID = '$help_id' LIMIT 1"));
  echo "
  <div align='center'>
   <br /><br />
   Are you sure you wish to delete the help file named <i>".$fetch_help['name']."</i>?<br />
   <a href='$PHP_SELF?action=do_delete&help_id=".$fetch_help['ID']."&view=' style='border: 1px solid red;'>Yes, do delete</a> <a href='javascript: history.go(-1);'>No, take me back</a>
  </div>";
 }else
 {
  echo "
  <div align='center'>
   <br /><br />
   Error: No help file specified. <a href='javascript: history.go(-1);'>Please go back</a>.
  </div>";
 }
 // End view "delete_help"
}elseif($_GET['view'] == "add_help" || $_POST['view'] == "add_help")
{
 // Start view "add_help"
 echo "<h1>Dragonsworn Help File - Add Help</h1>
  <a href='$PHP_SELF?view=' style='border: 1px solid darkblue;'>Help File Index</A><br />";
  if($_POST['action'] == 'add_help')
  {
   /*if($_POST['add_help_level'] > 0)
   {
    if($_POST['add_help_name'] != '')
    {
     if($_POST['add_help_keyword'] != '')
     {
      if($_POST['add_help_related'] != '')
      {
       if($_POST['add_help_catID'] > 0)
       {
        if($_POST['add_help_body'] != '')
        {*/
         $add_help = mysql_query("INSERT INTO helps (level, keyword, related, catID, body, name) VALUES ('".$_POST['add_help_level']."', '".$_POST['add_help_keyword']."', '".$_POST['add_help_related']."', '".$_POST['add_help_catID']."', '".$_POST['add_help_body']."', '".$_POST['add_help_name']."')");
         $add_help = "true";
/*        }else{$add_help = "invalid_body";}
       }else{$add_help = "invalid_catID";}
      }else{$add_help = "invalid_related";}
     }else{$add_help = "invalid_keyword";}
    }else{$add_help = "invalid_name";}
   }else{$add_help = "invalid_level";}*/
  }

  if($add_help != '')
  {
   echo "
   <div align='center'>
    <br /><br />";
   if($add_help != '')
   {
    if($add_help == 'true')
    {
     echo "Help file added successfully!";
    }elseif($add_help == 'invalid_body')
    {
     echo "Error: Please enter a valid body!";
    }elseif($add_help == 'invalid_catID')
    {
     echo "Error: Please enter a catID!";
    }elseif($add_help == 'invalid_name')
    {
     echo "Error: Please enter a name!";
    }elseif($add_help == 'invalid_level')
    {
     echo "Error: Please enter a level!";
    }elseif($add_help == 'invalid_related')
    {
     echo "Error: Please enter a valid related!";
    }elseif($add_help == 'invalid_keyword')
    {
     echo "Error: Please enter a valid keyword!";
    }else
    {
     echo "Error: $add_help";
    }
   }
   echo "
    <br /><br />
   </div>";
  }

  echo "
  <table width=100% cellpadding=5 cellspacing=1 style='border: 1px solid #000000;'>
   <form method='post' action='$PHP_SELF' style='padding: 0px 0px 0px 0px; margin: 0px 0px 0px 0px;'>
   <tr>
    <td style='border: 1px solid #000000;'><b>Level</b></td>
    <td style='border: 1px solid #000000;'><input type='text' size='30' name='add_help_level' value='";
    if($_POST['add_help_level'] != '' && $add_help != 'true')
    {
     echo $_POST['add_help_level'];
    }
    echo "'></td>
   </tr>
   <tr>
    <td style='border: 1px solid #000000;'><b>catID</b></td>
    <td style='border: 1px solid #000000;'><input type='text' size='30' name='add_help_catID' value='";
    if($_POST['add_help_catID'] != '' && $add_help != 'true')
    {
     echo $_POST['add_help_catID'];
    }
    echo "'></td>
   </tr>
   <tr>
    <td style='border: 1px solid #000000;'><b>Name</b></td>
    <td style='border: 1px solid #000000;'><input type='text' size='30' name='add_help_name' value='";
    if($_POST['add_help_name'] != '' && $add_help != 'true')
    {
     echo $_POST['add_help_name'];
    }
    echo "'></td>
   </tr>
   <tr>
    <td style='border: 1px solid #000000;'><b>Keywords</b></td>
    <td style='border: 1px solid #000000;'><textarea rows='5' cols='45' name='add_help_keyword'>";
    if($_POST['add_help_keyword'] != '' && $add_help != 'true')
    {
     echo stripslashes($_POST['add_help_keyword']);
    }else
    {
     echo stripslashes($fetch_help['keyword']);
    }
    echo "</textarea></td>
   </tr>
   <tr>
    <td style='border: 1px solid #000000;'><b>Related</b></td>
    <td style='border: 1px solid #000000;'><textarea rows='5' cols='45' name='add_help_related'>";
    if($_POST['add_help_related'] != '' && $add_help != 'true')
    {
     echo stripslashes($_POST['add_help_related']);
    }else
    {
     echo stripslashes($fetch_help['related']);
    }
    echo "</textarea></td>
   </tr>
   <tr>
    <td style='border: 1px solid #000000;'><b>Body</b></td>
    <td style='border: 1px solid #000000;'><textarea rows='30' cols='90' name='add_help_body' rows='5' cols='45'>";
    if($_POST['add_help_body'] != '' && $add_help != 'true')
    {
     echo stripslashes($_POST['add_help_body']);
    }else
    {
     echo stripslashes($fetch_help['body']);
    }
    echo "</textarea></td>
   </tr>
  </table>
  <div align='center'>
   <br /><br />";

   if($_POST['view'] != "")
   {
    $view = $_POST['view'];
   }else
   {
    $view = $_GET['view'];
   }

   if($_POST['help_id'] != "")
   {
    $help_id = $_POST['help_id'];
   }else
   {
    $help_id = $_GET['help_id'];
   }

   echo "
   <input type='hidden' name='action' value='add_help'><input type='hidden' name='view' value='$view'><input type='hidden' name='help_id' value='$help_id'>
   <input type='submit' value='Add A Help File'> <input type='reset' value='Reset Form'>
  </div>
  </form>";

 // end view "add_help"
}
echo "</body>
 </html>";
?>


