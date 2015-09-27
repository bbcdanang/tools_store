<?php

define( '_VALID_BBC', 1 );
define( '_ADMIN', '' );
if(isset($_GET['path']))
{
	if(!empty($_GET['path']))
	{
		$_GET['path'] .= (substr($_GET['path'], -1) != '/') ? '/' : '';
		$_SESSION['Mpath'] = $_GET['path'];
		header('Location:'.$_SERVER['PHP_SELF']);
	}
}
if(empty($_SESSION['Mpath']))
{
	if(!isset($_GET['path']))
	{
		header('Location:'.$_SERVER['PHP_SELF'].'?path=');
	}
	die('Masukkan path di URL ke PATH ROOT nya');
}
if(!is_file($_SESSION['Mpath'].'config.php'))
{
	unset($_SESSION['Mpath']);
	header('Location:'.$_SERVER['PHP_SELF'].'?path=');
}
$main = $_SESSION['Mpath'];

include $main.'config.php';
include $main.'includes/system/db.class.php';
include $main.'includes/system/db.connect.php';
$db->debug=1;
$fields = array();
show_link('clean_modules,clean_menu,clean_block,clean_config,clean_user,clean_lang,logout');
switch(@$_GET['id'])
{
	case 'clean_modules':
		$r = $db->getCol("SHOW TABLES");
		$tables = array();
		foreach($r AS $tbl)
		{
			$arr = $db->getCol("EXPLAIN `$tbl`");
			if(in_array('module_id', $arr))
			{
				if($arr[0] != 'module_id')
				{
					$tables[] = array($tbl, $arr[0]);
				}
			}
		}
		$fields['module_id']	= repair_tables('bbc_module', '', 'ORDER BY module_name ASC');
		foreach($tables AS $row)
		{
			$r_row = $db->getAll("SELECT `".$row[1]."`, `module_id` FROM `".$row[0]."` WHERE 1");
			foreach($r_row AS $d)
			{
				$n_module_id = @intval($fields['module_id'][$d['module_id']]['module_id']);
				if($n_module_id != $d['module_id'])
				{
					$db->Execute("UPDATE `".$row[0]."` SET module_id=$n_module_id WHERE `".$row[1]."`=".$d[$row[1]]."");
				}
			}
		}
	break;
	case 'clean_menu':
		$r_menu = array();
		$fields['cat_id']	= repair_tables('bbc_menu_cat');
		fetch_menu($r_menu , 1);
		fetch_menu($r_menu , 0);
		repair_tables(array('bbc_menu', $r_menu), 'menu_par_id', '', $fields);
	break;
	case 'clean_block':
		$fields['template_id']	= repair_tables('bbc_template', 'par_id', 'ORDER BY template_id ASC');
		$fields['position_id']	= repair_tables('bbc_block_position');
		$fields['block_ref_id']	= repair_tables('bbc_block_ref');
		$fields['theme_id']			= repair_tables('bbc_block_theme', 'par_id', '', $fields);
		$fields['block_id']			= repair_tables('bbc_block', 'par_id', '', $fields);
	break;
	case 'clean_config';
		repair_tables('bbc_config', '', 'ORDER BY module_id ASC');
	break;
	case 'clean_user';
		repair_tables('bbc_user_field', '', 'ORDER BY orderby ASC');
		$fields['user_id']	= repair_tables('bbc_user', 'par_id', 'ORDER BY user_id ASC');
		repair_tables('bbc_account', 'par_id', 'ORDER BY account_id ASC', $fields);
		$db->Execute("UPDATE `bbc_content` SET created_by=1, created_by_alias='admin', modified_by=1, revised=1, hits=0 WHERE 1");
	break;
	case 'clean_lang';
		$fields['ref_id']		= repair_tables('bbc_lang_ref', '', 'ORDER BY ref_id ASC');
		$fields['lang_id']	= repair_tables('bbc_lang', '', 'ORDER BY module_id, id ASC', $fields, 'lang_id');
		repair_tables('bbc_lang_translate', '', 'ORDER BY lang_id ASC', $fields);
		$db->Execute("update `bbc_lang` set lang=LOWER(lang) WHERE 1");
	break;
	case 'custom':
		if(!empty($_GET['table']))
		{
			$primary = !empty($_GET['primary']) ? $_GET['primary'] : 'id';
			repair_tables($_GET['table'], '', 'ORDER BY '.$primary.' ASC');
		}
	break;
	case 'salt':
		change_salt($_GET['new']);
	break;
	case 'install':
		install_domain(trim($_GET['domain']));
	break;
	case 'check_template':
	  check_template();
	break;
	case 'logout':
		header('Location:'.$_SERVER['PHP_SELF'].'?path=old');
	break;
	default:
		if(!empty($_GET['id']))
		{
			$tablename= $_GET['id'];
			$parfield	= @$_GET['par_id'];
			$ordered	= @$_GET['par_id'];
			$r_change = array();
			repair_tables($tablename, $parfield, $ordered, $r_change);
		}
	break;
}
echo implode("\n<hr />\n<hr />", (array)$Bbc->debug);
if(!empty($_GET['id']))
{
  clean_cache();
}
function show_link($r)
{
	$r = explode(',', $r);
	sort($r);reset($r);
	if(!empty($_GET['id']))
	{
		echo '<a href="'.$_SERVER['PHP_SELF'].'">&laquo; Back</a> | ';
	}
	$arr = array();
	foreach((array)$r AS $d)
	{
		$arr[] = '<a href="'.$_SERVER['PHP_SELF'].'?id='.$d.'">'.$d.'</a>';
	}
	echo implode(' | ', $arr);
	echo '  <b>'.$_SESSION['Mpath'].'</b>';
	$str = '';
	for($i=0;$i < 15;$i++)
	{
		$str .= md5(rand(0,255));
	}
	if(empty($_GET['id']))
	{
		?><br />
		name : <input type="text" id="table">
		primary: <input type="text" id="primary" value="id">
		<input type="submit" onclick="document.location.href='?id=custom&table='+document.getElementById('table').value+'&primary='+document.getElementById('primary').value;" value="submit">
		<br />
		_SALT: <input type="text" id="salt_new" value="<?php echo htmlentities(md5($str));?>">
		<input type="submit" onclick="document.location.href='?id=salt&new='+document.getElementById('salt_new').value;" value="submit">
		<pre style="display:inline;"><?php echo _SALT;?></pre>
		<br />
		Domain: <input type="text" id="domain" value="">
		<input type="submit" onclick="document.location.href='?id=install&domain='+document.getElementById('domain').value;" value="submit">
		<br />
    <form action="<?php echo $_SERVER['PHP_SELF'];?>?id=check_template" method="post">
      <b>Template</b><br />
    <?php
      global $db;
      $db->debug=0;
      $template = unserialize($db->getOne("SELECT params FROM bbc_config WHERE name='template' AND module_id=0"));
      $db->debug=1;
      include_once _ROOT.'includes/function/path.php';
      $path = _ROOT.'templates/'.$template.'/';
      $r = path_list($path.'css');
      foreach($r AS $i => $file)
      {
        if(preg_match('~\.css$~is', $file))
        {
          echo '<label><input type="checkbox" name="css['.$i.']" checked="checked" value="css/'.$file.'" /> '.$file.'</label><br />';
        }
      }
      echo '<br />';
      $r = path_list($path);
      foreach($r AS $file)
      {
        if(preg_match('~\.(?:php|html?)$~is', $file))
        {
          $i++;
          echo '<label><input type="checkbox" name="css['.$i.']" checked="checked" value="'.$file.'" /> '.$file.'</label><br />';
        }
      }
    ?>
      <input type="hidden" name="path" value="<?php echo $path;?>" />
      <input type="submit" name="change_email" value="Check" />
    </form>
		<?php
		echo _ROOT;
	}
}

function check_template()
{
  include_once _ROOT.'includes/function/path.php';
  function tools_check_template_image($arr, $add = '')
  {
    $output = array();
    foreach($arr AS $i => $dt)
    {
      if(is_array($dt))
      {
        $output = array_merge($output, tools_check_template_image($dt, $add.$i.'/'));
      }else{
        $output[] = $add.$dt;
      }
    }
    return $output;
  }
  if(!empty($_POST['path']))
  {
    $path = $_POST['path'];
    $notfound = array('css'=>array(), 'image'=>array());
    $data = array();
    $images = tools_check_template_image(path_list_r($path.'images/'), 'images/');
    foreach((array)$_POST['css'] AS $css)
    {
      $data[$css] = file_read($path.$css);
    }
    $is_check = 0;
    foreach($data AS $css => $text)
    {
      if($is_check)
      {
        $arr = $notfound['css'];
        $notfound['css'] = array();
      }else $arr = $images;
      $is_check++;
      foreach($arr AS $image)
      {
        if(!strstr($text, $image))
        {
          $notfound['css'][] = $image;
        }
      }
      preg_match_all('~(images/[^\"\'\s\)]+)~is', $text, $match);
      foreach((array)$match[1] AS $image)
      {
        if(!in_array($image, $notfound['image']) && !in_array($image, $images))
        {
          $notfound['image'][] = $image;
        }
      }
    }
    if(!empty($notfound['css']))
    {
      $line = count($notfound['css']);
      $width= strlen($line)-1;
      ?>
      <form action="<?php echo $_SERVER['PHP_SELF'];?>?id=check_template" method="post">
        <b>Files are exist, not found in templates OR css style</b><br />
        <textarea name="no" cols=<?php echo $width;?> rows="<?php echo $line;?>" style="float: left;"><?php for($i=1;$i<=$line;$i++) echo "\n".$i;?></textarea>
        <textarea name="deletes" cols=80 rows="<?php echo $line;?>" style="float: left;"><?php echo implode("\n", $notfound['css']);?></textarea>
        <input type="hidden" name="inpath" value="<?php echo $path;?>" /><br style="clear: both;" />
        <input type="submit" name="change_email" value="Delete Those Files" />
      </form>
      <?php
    }
    if(!empty($notfound['image']))
    {
      $line = count($notfound['image']);
      $width= strlen($line)-1;
      ?>
        <b>Image name found in css style OR templates, files are not exists</b><br />
        <textarea name="no" cols=<?php echo $width;?> rows="<?php echo $line;?>" style="float: left;"><?php for($i=1;$i<=$line;$i++) echo "\n".$i;?></textarea>
        <textarea name="notexists" cols=80 rows="<?php echo $line;?>" style="float: left;"><?php echo implode("\n", $notfound['image']);?></textarea>
        <br style="clear: both;" />
      <?php
    }
  }else{
    if(!empty($_POST['inpath']) AND !empty($_POST['deletes']))
    {
      $p = $_POST['inpath'];
      $r = explode("\n", $_POST['deletes']);
      $i = 0;
      foreach($r AS $file)
      {
        $file = $p.trim($file);
        if(is_file($file))
        {
          $i++;
          if(unlink($file))
            echo $i.' DELETE '.$file.'<br />';
        }
      }
    }
  }
}

function repair_tables($tablename, $parfield = 'par_id', $ordered= '', $r_change = array(), $out_id='')
{
	global $db, $Bbc;
	if(!empty($tablename[1]) && is_array($tablename[1]))
	{
		$arr = $tablename[1];
		$tablename = $tablename[0];
	}else{
		$arr = $db->getAssoc("SELECT * FROM `$tablename` WHERE 1 $ordered");
		if(empty($ordered))
		{
			ksort($arr);
			reset($arr);
		}
	}
	$parfield = empty($parfield) ? 'par_id' : $parfield;
	if(empty($arr)) return false;
	$fields	= $db->getCol("EXPLAIN `$tablename`");
	$db->Execute("TRUNCATE TABLE `$tablename`");
	if(count($fields) > 2)
	{
		$p_field= $fields[0];unset($fields[0]);
		if(empty($out_id)) $out_id = $p_field;
		foreach($arr AS $i => $data)
		{
			$data = addslashes_r($data);
			$insert = array();
			foreach($fields AS $f)
			{
				if($f != $p_field)
				{
					if($f == $parfield)
					{
						$data[$f] = @intval($arr[$data[$f]][$out_id]);
					}else{
						if(!empty($r_change[$f][$data[$f]][$f]))
						{
							$data[$f] = @intval($r_change[$f][$data[$f]][$f]);
						}
					}
					$insert[] = "`$f`='".$data[$f]."'";
				}
			}
			$db->Execute("INSERT INTO `$tablename` SET ".implode(',', $insert));
			$arr[$i][$out_id] = $db->Insert_ID();
		}
		$output = $arr;
	}else{
		$output = array();
		foreach($arr AS $i => $data)
		{
			$data = addslashes_r($data);
			$db->Execute("INSERT INTO `$tablename` SET `".$fields[1]."`='$data'");
			$output[$i] = array($fields[0] => $db->Insert_ID(), $fields[1] => $data);
		}
	}
	return $output;
}
function fetch_menu(&$output, $is_admin=0, $par_id = 0)
{
	global $db;
	$q = "SELECT * FROM bbc_menu WHERE menu_par_id=$par_id AND is_admin=$is_admin ORDER BY orderby ASC";
	$r = $db->getAssoc($q);
	foreach($r AS $i => $d)
	{
		$output[$i] = $d;
		fetch_menu($output, $is_admin, $i);
	}
}
function 	change_salt($code)
{
	global $db, $main;
	if(empty($code) || $code==_SALT) return false;
	$code = trim($code);
	$q = "SELECT account_id, user_id, password FROM bbc_account WHERE 1";
	$r = $db->getAll($q);
	foreach($r AS $d)
	{
#		$p0= decrypt($d['password'], _SALT);
		$p0= '123456';
		$p = encrypt($p0, $code);
		$q = "UPDATE bbc_account SET password='$p' WHERE account_id=".$d['account_id'];
		$db->Execute($q);
		$q = "UPDATE bbc_user SET password='".md5($p0)."' WHERE user_id=".$d['user_id'];
		$db->Execute($q);
		$db->dbOutput .= "\n".'<br />'.$p0.' -- '.$d['password'].' -- '.$p.'<br />';
	}
	$txt = file_read(_ROOT.'config.php');
	file_write(_ROOT.'config.php', str_replace(_SALT, $code, $txt));
	return true;
}
function encrypt($value, $salt)
{
	if(!$value){return false;}
	if(function_exists('mcrypt_get_iv_size'))	{
		$iv_size = mcrypt_get_iv_size(MCRYPT_RIJNDAEL_256, MCRYPT_MODE_ECB);
		$iv = mcrypt_create_iv($iv_size, MCRYPT_RAND);
		$output = mcrypt_encrypt(MCRYPT_RIJNDAEL_256, $salt, $value, MCRYPT_MODE_ECB, $iv);
	}else{
		$output = gzdeflate($value);
	}
	return trim(base64_encode($output)); //encode for cookie
}
function decrypt($value, $salt)
{
	if(!$value){return false;}
	$crypttext = base64_decode($value); //decode cookie
	if(function_exists('mcrypt_get_iv_size'))	{
		$iv_size = mcrypt_get_iv_size(MCRYPT_RIJNDAEL_256, MCRYPT_MODE_ECB);
		$iv = mcrypt_create_iv($iv_size, MCRYPT_RAND);
		$output = mcrypt_decrypt(MCRYPT_RIJNDAEL_256, $salt, $crypttext, MCRYPT_MODE_ECB, $iv);
	}else{
		$output = @gzinflate($crypttext);
	}
	return trim($output);
}
function file_read($file = '', $method = 'r')
{
	if ( ! file_exists($file))
	{
		return FALSE;
	}
	if (function_exists('file_get_contents'))
	{
		return file_get_contents($file);
	}
	if ( ! $fp = @fopen($file, $method))
	{
		return FALSE;
	}
	flock($fp, LOCK_SH);
	$data = '';
	if (filesize($file) > 0)
	{
		$data =& fread($fp, filesize($file));
	}
	flock($fp, LOCK_UN);
	fclose($fp);
	return $data;
}
function file_write($path, $data='', $mode = 'w+')
{
	if ( ! $fp = @fopen($path, $mode))
	{
		return FALSE;
	}
	flock($fp, LOCK_EX);
	fwrite($fp, $data);
	flock($fp, LOCK_UN);
	fclose($fp);
	@chmod($path, 0777);
	return TRUE;
}
function install_domain($new_domain)
{
	if(empty($new_domain)) return false;
	global $db;
	$q = "SELECT params FROM bbc_config WHERE module_id=0 AND name='site'";
	$r = @unserialize($db->getOne($q));
	if(empty($r)) return false;
	if(empty($r['url'])) return false;
	else $old_domain = $r['url'];
	$q = "SHOW TABLES";
	$r = $db->getCol($q);
	foreach($r AS $table)
	{
		$q = "SELECT * FROM `$table` WHERE 1";
		$ar= $db->getAll($q);
		foreach($ar AS $data)
		{
			$q = repair_data($data, $table, $old_domain, $new_domain);
			if(!empty($q)) $db->Execute($q);
		}
	}
}
function repair_data($data, $table, $old_domain, $new_domain)
{
	$output = array('primary' => false, 'data' => array());
	$update	= false;
	foreach($data AS $field => $value)
	{
		if(!$output['primary']) $output['primary'] = array($field, $value);
		else
		{
			$arr    = ($table=='bbc_block' || $table=='bbc_config' || $table=='bbc_account') ? @unserialize($value) : '';
			$value2 = repair_data_replace($arr, $old_domain, $new_domain, $value);
/*
			if(empty($arr))
			{
				$value2 = preg_replace('~'.$old_domain.'~is', $new_domain, $value);
			}else{
				if(is_array($arr))
				{
					$rr	= array();
					adodb_pr($arr);
					foreach($arr AS $f => $v)
					{
						if($f != 'register_groups')
						{
							$v_d = urldecode("$v");
							$use_encode = ($v_d == $v) ? false : true;
							$v = preg_replace('~'.$old_domain.'~is', $new_domain, $v_d);
							if($use_encode) $v = urlencode($v);
						}
						$rr[$f] = $v;
					}
				}else{
					$rr = preg_replace('~'.$old_domain.'~is', $new_domain, $arr);
				}
				$value2 = serialize($rr);
			}
*/
			$output['data'][] = "`$field`='".addslashes($value2)."'";
			if($value2 != $value) $update	= true;
		}
	}
	if(!$update) return false;
	else{
		$q = "UPDATE `$table` SET ".str_replace('\"', '"', implode(', ', $output['data']))." WHERE `".$output['primary'][0]."`=".$output['primary'][1];
		return $q;
	}
}
function repair_data_replace($arr, $old_domain, $new_domain, $value)
{
	if(empty($arr))
	{
		$value2 = preg_replace('~'.preg_quote($old_domain, '~').'~is', $new_domain, $value);
	}else{
		if(is_array($arr))
		{
			$rr	= array();
			foreach($arr AS $f => $v)
			{
				if($f != 'register_groups')
				{
					if(is_array($v))
					{
						$v = repair_data_replace($v, $old_domain, $new_domain, $value);
					}else{
						$v_d = urldecode($v);
						$use_encode = ($v_d == $v) ? false : true;
						$v = preg_replace('~'.preg_quote($old_domain, '~').'~is', $new_domain, $v_d);
						if($use_encode) $v = urlencode($v);
					}
				}
				$rr[$f] = $v;
			}
		}else{
			$rr = preg_replace('~'.$old_domain.'~is', $new_domain, $arr);
		}
		$value2 = serialize($rr);
	}
	return $value2;
}
function addslashes_r($vars)
{
	$vars = is_array($vars) ? array_map('addslashes_r', $vars) : addslashes($vars);
	return $vars;
}
function clean_cache()
{
	global $db;
  include_once _ROOT.'includes/function/path.php';
	$r = $db->getCol("SHOW TABLES");
	$tables = '`'.implode('`, `', $r).'`';
	$db->Execute("REPAIR TABLE $tables");
	$db->Execute("FLUSH TABLE $tables");
	path_delete(_ROOT.'images/cache');
}
