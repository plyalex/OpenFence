// Creates the gservice factory. This will be the primary means by which we interact with Google Maps
angular.module('gservice', [])
    .factory('gservice', function($rootScope, $http){ 

        // Initialize Variables
        // -------------------------------------------------------------
        // Service our factory will return
        var googleMapService = {};

        // Array of locations obtained from API calls
        var locations = [];


        // Selected Location (initialize to center of Australia)
        var selectedLat = -27;
        var selectedLong = 133;
    
        // Handling Clicks and location selection
        googleMapService.clickLat  = 0;
        googleMapService.clickLong = 0;
            // If map has not been created...


        // Functions
        // --------------------------------------------------------------
        // Refresh the Map with new data. Takes three parameters (lat, long, and filtering results)
        googleMapService.refresh = function(latitude, longitude, filteredResults){

            // Clears the holding array of locations
            locations = [];

            // Set the selected lat and long equal to the ones provided on the refresh() call
            selectedLat = latitude;
            selectedLong = longitude;
            
            // If filtered results are provided in the refresh() call...
            if (filteredResults){

                // Then convert the filtered results into map points.
                locations = convertToMapPoints(filteredResults);

                // Then, initialize the map -- noting that a filter was used (to mark icons yellow)
                initialize(latitude, longitude, true);
            } else {
                // Perform an AJAX call to get all of the records in the db.
                $http.get('/fencepoints').success(function(response){  
                    // Then convert the results into map points
                    locations = convertToMapPoints(response); 
                    initialize(latitude, longitude, false);
                }).error(function(){});
                
                // Then initialize the map -- noting that no filter was used.

            }
        };
    
        googleMapService.refreshAnimals = function(latitude, longitude, filteredResults){
            locations = [];
            
            // Loop through all of the JSON entries provided in the response
            for(var i= 0; i < filteredResults.length; i++) {
                var animalpoint = filteredResults[i];
                var  contentString =
                    '<p><b>Paddock</b>: ' + animalpoint.paddock +
                    '<br><b>Name</b>: ' + animalpoint.animal +
                    '</p>';
                
                // Converts each of the JSON records into Google Maps Location format (Note [Lat, Lng] format).
                locations.push({
                    latlon: new google.maps.LatLng(animalpoint.location[1], animalpoint.location[0]),
                    message: new google.maps.InfoWindow({
                        content: contentString,
                        maxWidth: 320
                    }),
                    paddock: animalpoint.paddock,
                    animal: animalpoint.animal,
                    ANnotFE: true
                });
                
            };
            // Perform an AJAX call to get all of the records in the db.
                $http.get('/fencepoints').success(function(response){  
                    // Then convert the results into map points
                    var temp=convertToMapPoints(response)
                    temp.forEach(function(n, i){
                        locations.push(temp[i]); 
                    });
                    initialize(latitude, longitude, false);
                }).error(function(){});
            
        };
        

        // Private Inner Functions
        // --------------------------------------------------------------
        // Convert a JSON of fencepoints into map points
        var convertToMapPoints = function(response){
            // Clear the locations holder
            var locationsConvert = [];
            
            
            // Loop through all of the JSON entries provided in the response
            for(var i= 0; i < response.length; i++) {
                var fencepoint = response[i];

                // Create popup windows for each record
                var  contentString =
                    '<p><b>Paddock</b>: ' + fencepoint.paddock +
                    '<br><b>Point</b>: ' + fencepoint.order +
                    '<br><b>Version</b>: ' + fencepoint.version +
                    '</p>';

                // Converts each of the JSON records into Google Maps Location format (Note [Lat, Lng] format).
                locationsConvert.push({
                    latlon: new google.maps.LatLng(fencepoint.location[1], fencepoint.location[0]),
                    message: new google.maps.InfoWindow({
                        content: contentString,
                        maxWidth: 320
                    }),
                    paddock: fencepoint.paddock,
                    order: fencepoint.order,
                    version: fencepoint.version,
                    ANnotFE: false
                });
            }

        // location is now an array populated with records in Google Maps format
        return locationsConvert;
    };
    

            
     // Initializes the map
    var oldZoom = 15;
    var initialize = function(latitude, longitude, filter) {

        // Uses the selected lat, long as starting point
        var myLatLng = {lat: selectedLat, lng: selectedLong};
        var test = new Array();
        // If map has not been created...
        if (!map){
            // Create a new map and place in the index.html page
                var map = new google.maps.Map(document.getElementById('map'), {
                zoom: oldZoom,
                center: myLatLng,
                mapTypeId: 'satellite',
                streetViewControl: false
                //disableDefaultUI: true
            });     
        }

        google.maps.event.addListener(map, "zoom_changed", function() { oldZoom = map.getZoom(); });
        // Loop through each location in the array and place a marker
        var val=0;
        locations.forEach(function(n, i){
            if(n.ANnotFE){
                var icon = "http://maps.google.com/mapfiles/ms/icons/yellow-dot.png";
                var marker = new google.maps.Marker({
                    position: n.latlon,
                    map: map,
                    title: "Big Map",
                    icon: icon
                });
            }else{
                var marker = new google.maps.Marker({
                    position: n.latlon,
                    map: map,
                    title: "Big Map",
                    label: n.order.toString()
                }); 
                test[val] =  n.latlon;
                val = val+1;
            }
            
            // For each marker created, add a listener that checks for clicks
            google.maps.event.addListener(marker, 'click', function(e){
                // When clicked, open the selected marker's message
                currentSelectedMarker = n;
                n.message.open(map, marker);
            });
        });

        var fenceExterior = new google.maps.Polygon({
              paths: test,
              strokeColor: '#0000FF',
              strokeOpacity: 0.8,
              strokeWeight: 3,
              fillColor: '#0000FF',
              fillOpacity: 0.1
            });

        fenceExterior.setMap(map);

        // Set initial location as a bouncing red marker
        var initialLocation = new google.maps.LatLng(latitude, longitude);
        var marker = new google.maps.Marker({
            position: initialLocation,
            animation: google.maps.Animation.BOUNCE,
            map: map,
            icon: 'http://maps.google.com/mapfiles/ms/icons/red-dot.png'
        });
        lastMarker = marker;

        // Function for moving to a selected location
        map.panTo(new google.maps.LatLng(latitude, longitude));

        // Clicking on the Map moves the bouncing red marker
        google.maps.event.addListener(map, 'click', function(e){
            var marker = new google.maps.Marker({
                position: e.latLng,
                animation: google.maps.Animation.BOUNCE,
                map: map,
                icon: 'http://maps.google.com/mapfiles/ms/icons/red-dot.png'
            });

            // When a new spot is selected, delete the old red bouncing marker
            if(lastMarker){
                lastMarker.setMap(null);
            }

            // Create a new red bouncing marker and move to it
            lastMarker = marker;
            map.panTo(marker.position);

            // Update Broadcasted Variable (lets the panels know to change their lat, long values)
            googleMapService.clickLat = marker.getPosition().lat();
            googleMapService.clickLong = marker.getPosition().lng();
            $rootScope.$broadcast("clicked");
        });
    };
    

    // Refresh the page upon window load. Use the initial latitude and longitude
    google.maps.event.addDomListener(window, 'load', googleMapService.refresh(selectedLat, selectedLong));

    return googleMapService;         
});
