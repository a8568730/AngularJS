var app = angular.module('demo', [])
		  .controller('WelcomeController', function($scope) {
		      $scope.greeting = 'Welcome!';
		      $scope.profiles = [
		          {
		    		  name: "小M",
		    		  locate: "Taiwan Hualien"
		          },
		          {
		    		  name: "Ihc",
		    		  locate: "Taiwan Zanhua"
		          }
		      ];
		  })
		  .controller('TodoController', function($scope){
		      $scope.remaining = 0;
		      $scope.todolist = [
		           {
		        	   text: "wash the car", 
		        	   done: false
		           },
		           {
		        	   text: "put things in order",
		        	   done: false
		           }
		      ];
		      
		      $scope.remaining = function(){
		    	  //count false number
		    	  var count = 0;
		    	  angular.forEach($scope.todolist, function(item){
		    		  	count += item.done? 0 : 1;
		    	  });
		    	  return count;
		      }; 
		      
		      $scope.refresh = function(){
		    	 var oldlist = $scope.todolist;
		    	 $scope.todolist = [];
		    	 angular.forEach(oldlist, function(olditem){
		    		 if(olditem.done === false){
		    			 $scope.todolist.push(olditem);
		    		 }
		    	 });
		      };
		      
		      $scope.newtext = "";		      
		      $scope.addnew = function(){
		    	  $scope.todolist.push({text: $scope.newtext, done: false});
		    	  $scope.newtext = "";
		      };
		  });