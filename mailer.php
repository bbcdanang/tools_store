<?php  if (!defined('_VALID_BBC')) exit('No direct script access allowed');

if(!isset($_POST['Submit']))
{
	$q = "SELECT id, name FROM bbc_module ORDER BY name";
?>
<form action="" method="POST" enctype="multipart/form-data" target="output">
	<table width="100%" height="100%" border=0 cellpadding="0" cellspacing="0">
		<tr>
			<td width="100px">template</td>
			<td><input type=text name="template" value="" size="40"></td>
		</tr>
		<tr>
			<td>Module</td>
			<td><select name="module_id"><?php echo createOption($db->getAssoc($q), '');?></select></td>
		</tr>
		<tr>
			<td>mail to</td>
			<td><input type=text name="mail_to" value="" size="40"></td>
		</tr>
		<tr>
			<td>params</td>
			<td style="height:100px;">
				<textarea name="params" style="width: 100%;border: 1px solid #ccc;" rows=10>$params = array(

);</textarea>
			</td>
		</tr>
		<tr bgcolor=#f0f0f0>
			<td></td>
			<td style="height:10px;">
				<input type=submit name="Submit" value="Execute">
			</td>
		</tr>
		<tr>
			<td colspan=2>
				<iframe src="" name="output" width="100%" height="100%" frameborder=0></iframe>
			</td>
		</tr>
	</table>
</form>
<?php
} else {
	@eval(stripslashes($_POST['params']));
	$sys->module_id = $_POST['module_id'];
	$to = array();
	$r = explode(',', $_POST['mail_to']);
	foreach($r AS $t) {
		$r = explode(';', $t);
		foreach($r AS $t) {
			if(is_email($t))
				$to[] = $t;
		}
	}
	@extract($params);
	$sys->mail_send($to
	, $_POST['template']
	, $debug = true);
}
?>