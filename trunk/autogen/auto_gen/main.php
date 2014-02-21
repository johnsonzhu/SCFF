<?php
	if($argc < 2)
	{
		echo "Usage : ".$argv[0]." xml template.php need_split index \n";
		exit(0);
	}
	
	include 'xml_parser.php';

	$p = new xmlParser();
	$p->parse($argv[1]);
	//print_r($p->output);
    
	$category_attrs = $p->output[0]['child'][0]['attrs'];
	$svc_h = basename($category_attrs[SVCH]);
	$svc_no_h = basename($svc_h, '.h');
    $class_declare = $category_attrs[CLASSDECLARE];
    $param_def_h = $category_attrs[PARAMDEFH];
    $need_init = $category_attrs[NEEDINIT];
    $handle_sys_err = $category_attrs[HANDLESYSERR];
    $on_timer = $category_attrs[ONTIMER];

	$category = $category_attrs[NAME];
	$category_lower = strtolower($category_attrs[NAME]);
	$category_upper = strtoupper($category);
	$category_namespace = $category_attrs[NAMESPACE];

	$statful_services = $p->output[0]['child'][0]['child'];
	$service_name = $statful_services[$i]['attrs'][NAME];
    $request_items = $statful_services[$i]['child'][0]['child'];
    $response_items = $statful_services[$i]['child'][1]['child'];

	$map_type4offset["uint8_t"]  = 1; // c
	$map_type4offset["int8_t"]   = 1; // c
	$map_type4offset["uint16_t"] = 1; // w
	$map_type4offset["int16_t"]  = 1; // w
	$map_type4offset["uint32_t"] = 2; // dw
	$map_type4offset["int32"]  = 2;   // dw
	$map_type4offset["uint64_t"] = 3; // ddw
	$map_type4offset["int64_t"]  = 3; // ddw
	$map_type4offset["std::string"] = 1; // s

	include $argv[2];
?>

