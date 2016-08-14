// Creates the addCtrl Module and Controller. Note that it depends on the 'geolocation' module and service.
var trackingCtrl = angular.module('trackingCtrl', ['geolocation']);
trackingCtrl.controller('trackingCtrl', function($scope, $http, $rootScope, geolocation, gservice){
    
    //Initialise variables
    $scope.formData = {};
    // Set initial coordinates to the center of the US
    $scope.formData.paddock = 0;
    $scope.formData.point = 10;
    $scope.formData.version = 0;
    $scope.formData.latitude = -27.000000;
    $scope.formData.longitude = 133.000000;
    
    
    // Get User's actual coordinates based on HTML5 at window load
    geolocation.getLocation().then(function(data){
        // Set the latitude and longitude equal to the HTML5 coordinates
        coords = {lat:data.coords.latitude, long:data.coords.longitude};

        // Display coordinates in location textboxes rounded to three decimal points
        $scope.formData.longitude = parseFloat(coords.long).toFixed(6);
        $scope.formData.latitude = parseFloat(coords.lat).toFixed(6);
        // Set the latitude and longitude equal to the HTML5 coordinates
        $.getJSON( '/tracking/list', function( data ) {
                gservice.refreshAnimals($scope.formData.latitude, $scope.formData.longitude, data);
        });
    });
    
    
    $.getJSON( '/tracking/list', function( data ) {
        gservice.refreshAnimals($scope.formData.latitude, $scope.formData.longitude, data);
    });
        
    var now = new Date;
    now.setMilliseconds(0);
    now.setSeconds(0);
    $scope.formData.newestDate = now;
    //now.setMonth(now.getMonth()-1);
    //$scope.formData.oldestDate = now;
    
    
    
    // Functions
    // ----------------------------------------------------------------------------
    
    
    function updateList(data){
        var fencePoints = [];
            $scope.fencePoints = data;
            $scope.formData.point = $scope.fencePoints.length;
            $scope.formData.version = $scope.fencePoints[0].version ;  //** Has console error if there is no entries in database
        gservice.refresh($scope.formData.latitude, $scope.formData.longitude, false);
    };
    
    // Get coordinates based on mouse click. When a click event is detected....
    $rootScope.$on("clicked", function(){

        // Run the gservice functions associated with identifying coordinates
        $scope.$apply(function(){
            $scope.formData.latitude = parseFloat(gservice.clickLat).toFixed(6);
            $scope.formData.longitude = parseFloat(gservice.clickLong).toFixed(6);
        });
    });
    
    $scope.refresh = function(){
        $.getJSON( '/tracking/list', function( data ) {
            console.log(data);
            gservice.refreshAnimals($scope.formData.latitude, $scope.formData.longitude, data);
        });
    };   
    
    var queryBody = {};
    // Take query parameters and incorporate into a JSON queryBody
    $scope.queryAnimals = function(){

        // Assemble Query Body
        queryBody = {
            oldestDate: $scope.formData.oldestDate,
            newestDate: $scope.formData.newestDate,
            animalID  : $scope.formData.animalID
        };

        
        // Post the queryBody to the /query POST route to retrieve the filtered results
        $http.post('/tracking/list', queryBody)
            
            // Store the filtered results in queryResults
            .success(function(queryResults){
                console.log(queryResults);
                // Pass the filtered results to the Google Map Service and refresh the map
                gservice.refreshAnimals($scope.formData.latitude, $scope.formData.longitude, queryResults);

                // Count the number of records retrieved for the panel-footer
                $scope.queryCount = queryResults.length;
            })
            .error(function(queryResults){
                console.log('Error ' + queryResults);
            })
    };
    
    $scope.send2Devices =function() {
        
    };
   
});