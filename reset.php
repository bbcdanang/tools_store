<?php  if (!defined('_VALID_BBC')) exit('No direct script access allowed');

$all = 'all';
$_POST['username'] = isset($_POST['username']) ? $_POST['username'] : $all;
$_POST['password'] = isset($_POST['password']) ? $_POST['password'] : '123456';
?>
<form action="" method="POST" enctype="multipart/form-data">
<h2>RESET PASSWORD</h2>
<table border="1">
  <tr>
    <td>Username</td>
    <td><input type="text" name="username" value="<?php echo @$_POST['username'];?>"></td>
  </tr>
  <tr>
    <td>Password</td>
    <td><input type="text" name="password" value="<?php echo @$_POST['password'];?>"></td>
  </tr>
  <tr>
    <td>&nbsp;</td>
    <td><input type="submit" name="Submit" value="Submit"></td>
  </tr>
</table>
</form>
<?php
if(isset($_POST['Submit']))
{
	$db->debug=1;
	if(@$_POST['username'] == $all)
	{
		$q = "UPDATE bbc_user SET password='".encode($_POST['password'])."'";
	}else{
		$q = "UPDATE bbc_user SET password='".encode($_POST['password'])."' WHERE username='".$_POST['username']."'";
	}
	$db->Execute($q);

	echo '<br />Affected Rows : '.$db->Affected_rows().' rows';
	echo '<br />'.$db->dbOutput;
}