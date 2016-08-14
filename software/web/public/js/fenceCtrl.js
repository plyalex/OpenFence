// Creates the addCtrl Module and Controller. Note that it depends on the 'geolocation' module and service.
var fenceCtrl = angular.module('fenceCtrl', ['geolocation']);
fenceCtrl.controller('fenceCtrl', function($scope, $http, $rootScope, geolocation, gservice){
    
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
        gservice.refresh(data.coords.latitude, data.coords.longitude, false);
    });
    
    
    // jQuery AJAX call for JSON
    $.getJSON( '/fencepoints', function( data ) {
        updateList(data);
    });
    

    
    
    
    // Functions
    // ----------------------------------------------------------------------------
    
    
    function updateList(data){
        var fencePoints = [];
            $scope.fencePoints = data;
            console.log($scope.fencePoints);
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
    
    // Creates a new point based on the form fields
    $scope.addPoint = function() {

        // Grabs all of the text box fields
        var pointData = {
            paddock: $scope.formData.paddock,
            order: $scope.formData.point,
            version: $scope.formData.version,
            location: [$scope.formData.longitude, $scope.formData.latitude]
        };
        console.log("Success grabbing variables")
        

        // Saves the user data to the db
        $http.post('/fencepoints/add', pointData)
            .success(function (data) {

                // Once complete, clear the form (except location)
                updateList(data);
            
                // Refresh the map with new data
                //gservice.refresh($scope.formData.latitude, $scope.formData.longitude);
                
            })
            .error(function (data) {
                console.log('Error: ' + data);
            });
    };
    
    $scope.deletePoint = function(id2Delete) {
        var jsondata= "{\x22_id\x22:\x22"+id2Delete+"\x22}";// ObjectId(
        // Saves the user data to the db
        $http.post('/fencepoints/delete', jsondata)
            .success(function (data) {

                // Once complete, clear the form (except location)
                updateList(data);
            
                // Refresh the map with new data
                //gservice.refresh($scope.formData.latitude, $scope.formData.longitude);
                
            })
            .error(function (data) {
                //console.log('Error: ' + data);
                //updateList();
            });
          //updateList();
    };
    
    $scope.send2Devices =function() {
        
    };
   
});