<?php  if (!defined('_VALID_BBC')) exit('No direct script access allowed');

if (!function_exists('_func')) {
	function _func($file)
	{
		if(!empty($file))
		{
			_ext($file);
			$path = str_replace('.php', '', $file);
			$files= array();
			if(file_exists(_ROOT.'modules/'.$path.'/_function.php'))
			{
				$files[] = _ROOT.'modules/'.$path.'/_function.php';
			  include_once current($files);
			}
			if(is_file(_FUNC.$file))
			{
				$files[] = _FUNC.$file;
				include_once current($files);
			}
			$j = func_num_args();
			if ($j > 1)
			{
				$func = $path.'_'.func_get_arg(1);
				if (function_exists($func))
				{
					if ($j > 2)
					{
						$param = array();
						for($i=2;$i < $j;$i++)
						{
							$param[] = func_get_arg($i);
						}
						return call_user_func_array($func, $param);
					}else{
						return $func();
					}
				}else{
					$msg = 'Function "'.$func.'" not found';
					if (!empty($files)) {
						$msg .= ' in '.implode(' and ', $files);
					}
					die($msg.' !');
				}
			}
		}
	}
}
if (!function_exists('_class')) {
	function _class($file)
	{
		global $Bbc;
		if(!empty($file))
		{
			$class = preg_replace('~\.php~s', '', $file);
			if(isset($Bbc->$class) && $Bbc->$class != false) return $Bbc->$class;
			_ext($file);
			$filename = '';
			if(is_file(_CLASS.$file))
			{
			  $filename = _CLASS.$file;
			}else
			if(file_exists(_ROOT.'modules/'.str_replace('.php', '/_class.php', $file)))
			{
			  $filename = _ROOT.'modules/'.str_replace('.php', '/_class.php', $file);
			  $class .= '_class';
			}
			if(!empty($filename))
			{
				include_once $filename;
				if (class_exists($class))
				{
					$j = func_num_args();
					if($j > 1)
					{
						$l = array();
						for($i=1;$i < $j;$i++)
						{
							$k = 'l'.$i;
							$$k = func_get_arg($i);
							$l[] = '$'.$k;
						}
						eval('$Bbc->'.$class.' = new '.$class.'('.implode(',', $l).');');
					}else{
						$Bbc->$class = new $class();
					}
				}else $Bbc->$class = false;
			}
		}
		return $Bbc->$class;
	}
}
if (!function_exists('_lib')) {
	function _lib($file)
	{
		global $Bbc;
	#	if(isset($Bbc->$file)) return $Bbc->$file;
		if(!empty($file))
		{
			$class = preg_replace('~\.php~s', '', $file);
			_ext($file);
			if(is_file(_LIB.$class.'/'.$file))
			{
				include_once _LIB.$class.'/'.$file;
			}
			if (class_exists($class))
			{
				$j = func_num_args();
				if($j > 1)
				{
					$l = array();
					for($i=1;$i < $j;$i++)
					{
						$k = 'l'.$i;
						$$k = func_get_arg($i);
						$l[] = '$'.$k;
					}
					eval('$Bbc->'.$class.' = new '.$class.'('.implode(',', $l).');');
				}else{
					$Bbc->$class = new $class();
				}
			}else $Bbc->$class = false;
		}
		return $Bbc->$class;
	}
}
if (!function_exists('_ext')) {
	function _ext(&$file)
	{
		if(substr($file, -4) != '.php') $file .= '.php';
	}
}
$notFile= array('_switch.php', 'index.php');
$M_DIR	= dirname(__FILE__).'/';
if(!file_exists($M_DIR)){
echo $M_DIR;
die();
}else	chdir($M_DIR);
$sys->stop();
switch( $Bbc->mod['task'] )
{
	case 'main' :
	ob_start();
	?>
		<table>
		  <tr>
		    <td style="width:200px;">
		    	<IFRAME name="navigation" src="<?php echo site_url($Bbc->mod['circuit'].'.list');?>" frameBorder="0" width="100%" height="100%" scrolling="auto"></IFRAME>
		    </td>
		    <td>
		    	<IFRAME name="tasks" src="<?php echo site_url($Bbc->mod['circuit'].'.PHP');?>" frameBorder="0" width="100%" height="100%" scrolling="auto"></IFRAME>
		    </td>
		  </tr>
		</table>
	<?php
	$output = ob_get_contents();
	ob_end_clean();
	show_css($output);
	break;
	case 'list' :
	ob_start();
		echo '<div style="float: left;"><a href="" onClick="window.location.reload( true );return false" title="Refresh Left">V refresh</a></div>';
		echo '<div style="float:right;"><a href="" onClick=\'window.parent.frames["tasks"].window.location.reload( true );return false\' title="Refresh right"><b>&gt;</b> refresh</a></div>';
		echo '<br /><br />';
		if ($dir = @opendir($M_DIR))
		{
			$r_data = array();
			while (($data = readdir($dir)) !== false)
			{
				if(is_file($M_DIR.$data)
					&& !in_array($data, $notFile)
					&& substr(strtolower($data),-4)=='.php')
				{
					$r_data[] = preg_replace('~\.php$~is', '', $data);
				}
			}
			closedir($dir);
			asort ($r_data);
		}

		echo "<ul>";
		foreach((array)$r_data as $data)
		{
			echo "<li><a href=\"".$Bbc->mod['circuit'].".$data\" target=\"tasks\">$data</a></li>";
		}
		echo "</ul>";
	$output = ob_get_contents();
	ob_end_clean();
	show_css($output);
	break;
	default:
		$file = $Bbc->mod['task'].'.php';
		if(is_file($file)) {
			include $file;
		}
	break;
}

function show_css($data = '')
{
?>
<html>

<head>
<title>Test Script List</title>
<style type="text/css">
body{
margin: 0px;
padding: 0px;
font-family:verdana, arial, sans-serif;
font-size: 12px;
color: #666666
}
table{
margin: 0px;
padding: 0px;
width: 100%;
height: 100%;
border: 0px solid #307b9a;
}
td{
vertical-align:top;
}

ul {
clear: both;
list-style: dotted;
margin: 0px !important;
padding: 0px !important;
}
ul li{
padding-top: 5px !important;
padding-left: 2px !important;
}
a{
color: #666666;
text-decoration: none;
border-bottom: 1px #ccc dotted;
}
a:hover{
color: #a00000;
text-decoration: none;
}
a:active{
color: #ff0000;
text-decoration: none;
}
</style>
</head>
<body bgcolor="#ffffff">
	<?php echo $data;?>
</body>
</html>
<?php
}
