
 //jquery ver.
 $(document).ready(function(){
	var appp1 = angular.module('app1',[]);
	var appp2 = angular.module('app2',[]);

	angular.bootstrap(document.getElementById('app1'),['app1']);
	angular.bootstrap(document.getElementById('app2'),['app2']);
	
});


/*
 //javascript ver.  
 window.onload = function(){
	var appp1 = angular.module('app1',[]);
	var appp2 = angular.module('app2',[]);

	angular.bootstrap(document.getElementById('app1'),['app1']);
	angular.bootstrap(document.getElementById('app2'),['app2']);
	
};*/