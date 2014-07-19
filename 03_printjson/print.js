var app = angular.module('demo', [])
		  .controller('WelcomeController', function($scope) {
		      $scope.greeting = 'Welcome!';
		      $scope.profiles = [
		          {
		    		  name:"¤pM",
		    		  locate:"Taiwan Hualien"
		          },
		          {
		    		  name:"Ihc",
		    		  locate:"Taiwan Zanhua"
		          }
		      ];
		  });