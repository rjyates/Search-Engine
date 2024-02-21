<!--      Start the HTML Page with a comment         -->
<!--      Print out the title, etc. and opening tags --> 
<html>
<head><title>Search Engine</title></head>
<body>

<!--      Here comes the php portion of the page     -->
<?php

//------------------------------------------------------
// Now a php comment 
// This is the main program that either processes the form
// data or prints the form to capture data
//------------------------------------------------------
if (isset($_POST['stage']) && ($_POST['stage'] == 'process'))
{
   process_form();
}
else
{
   print_form();
}

//------------------------------------------------------
// This is the function that prints the form
//------------------------------------------------------
function print_form()
{
echo <<<END
   <center><h4>Search Engine</h4></center>
   <form action="$_SERVER[PHP_SELF]" method="post" id="form1">
   <table border="0">
     <tr>
       <td>query: </td>
	   <td><input type="Text" name="query" align="TOP" size="20"></td>
	   <td><input type="hidden" name="stage" value="process"</td>
	   <td><input type="Submit" name="submit" value="Search" align="MIDDLE"></td>
	 </tr>
    </table></form>
END;
}

//------------------------------------------------------
// This is the function that processes the form data
//------------------------------------------------------
function process_form()
{
   echo "<h2>Process query: " . $_POST[query] . "</h2>";

   $command =   "./retrieve " . $_POST[query] . " " . "filesout/";
  // echo ($command);
   echo exec ($command);

//   echo "<input type="button" value="back" onclick="history.go(-1)">";  
}

?>
<input type="button" value="back" onclick="history.go(-1)">
</body>
<!--<input type="button" value="back" onclick="history.go(-1)">-->
</html>

