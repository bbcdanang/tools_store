<?php  if (!defined('_VALID_BBC')) exit('No direct script access allowed');

function encode($value, $salt='')
{
	if(!$value){return false;}
	if (empty($salt)) {
		$salt = _SALT;
	}
	if(function_exists('mcrypt_get_iv_size'))	{
		$iv_size = mcrypt_get_iv_size(MCRYPT_RIJNDAEL_256, MCRYPT_MODE_ECB);
		$iv = mcrypt_create_iv($iv_size, MCRYPT_RAND);
		$output = mcrypt_encrypt(MCRYPT_RIJNDAEL_256, $salt, $value, MCRYPT_MODE_ECB, $iv);
	}else{
		$output = gzdeflate($value);
	}
	return trim(base64_encode($output)); //encode for cookie
}

function decode($value, $salt='')
{
	if(!$value){return false;}
	if (empty($salt)) {
		$salt = _SALT;
	}
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

include_once str_replace('tools_store', 'tools', __FILE__);