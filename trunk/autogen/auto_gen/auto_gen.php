<?php
    if($argc < 2)
	{
		echo "Usage : ".$argv[0]." xml \n";
		exit(0);
	}

    $adapter_tmpl="category/lib/webservice_adaptor.cpp.php";
    if ($argc > 2)
    {
       echo "mode is: $argv[2]\n";
       if ("$argv[2]" == "async")
       {
	   echo "auto gen for async web mode\n";
    	   $adapter_tmpl="category/lib/webservice_adaptor_async.cpp.php";
       }
    }
   
    
	
	include 'xml_parser.php';
	$p = new xmlParser();
	$p->parse($argv[1]);
	//print_r($p->output);
	
	$category_attrs = $p->output[0]['child'][0]['attrs'];
    $output_dir = $category_attrs['DIR'].'/';
	$category = strtolower($category_attrs['NAME']);
	$category_type = strtolower($category_attrs['TYPE']);
	
    @mkdir($output_dir);
	@mkdir($output_dir.$category);
//    @mkdir($output_dir.$category.'/include/');
    @mkdir($output_dir.$category.'/lib/');
    @mkdir($output_dir.$category.'/src/');
    @mkdir($output_dir.$category.'/cgi/');
    @mkdir($output_dir.$category.'/websvc/');
    
	print("Generating ".$output_dir.'/'.$category.'/lib/'.$category.'_adaptor.cpp'."\n");
   $cmd='./php447 -q main.php '.$argv[1].' '.$adapter_tmpl.' > '.$output_dir.'/'.$category.'/lib/'.$category.'_adaptor.cpp';
   echo "$cmd \n";
    shell_exec("$cmd");
	
	print("Generating ".$output_dir.'/'.$category.'/cgi/Makefile.org'."\n");
    shell_exec('./php447 -q main.php '.$argv[1].' category/cgi/Makefile.php > '.$output_dir.'/'.$category.'/cgi/Makefile.org');

	print("Generating ".$output_dir.'/'.$category.'/websvc/Makefile.org'."\n");
    shell_exec('./php447 -q main.php '.$argv[1].' category/websvc/Makefile.php > '.$output_dir.'/'.$category.'/websvc/Makefile.org');

	print("Generating ".$output_dir.'/'.$category.'/src/'.$category.'_interceptor.h'."\n");
    shell_exec('./php447 -q main.php '.$argv[1].' category/src/interceptor.h.php > '.$output_dir.'/'.$category.'/src/'.$category.'_interceptor.h');

	print("Generating ".$output_dir.'/'.$category.'/src/'.$category.'_interceptor.cpp.org'."\n");
    shell_exec('./php447 -q main.php '.$argv[1].' category/src/interceptor.cpp.php > '.$output_dir.'/'.$category.'/src/'.$category.'_interceptor.cpp.org');

	print("Generating ".$output_dir.'/'.$category.'/Makefile'."\n");
    shell_exec('./php447 -q main.php '.$argv[1].' category/Makefile > '.$output_dir.'/'.$category.'/Makefile');
	
    print("Done!\n")
?>
