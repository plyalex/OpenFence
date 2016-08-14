// Dependencies
var mongoose        = require('mongoose');
var Fence            = require('./model.js');

// Opens App Routes
module.exports = function(app) {

    // GET Routes
    // --------------------------------------------------------
    // Retrieve records for all users in the db
    app.get('/fencepoints', function(req, res){

        // Uses Mongoose schema to run the search (empty conditions)
        var query = Fence.find({});
        query.setOptions({sort: "order"});
        query.exec(function(err, fencepoints){
            if(err)
                res.send(err);

            // If no errors are found, it responds with a JSON of all users
            res.json(fencepoints);
        });
    });

    // POST Routes
    // --------------------------------------------------------
    // Provides method for saving new users in the db
    app.post('/fencepoints/add', function(req, res){
        console.log("Adding a new Point")
        // Creates a new Fence point based on the Mongoose schema and the post body.
        var newpoint = new Fence(req.body);
        console.log(req.body)
        // New Fence is saved in the db.
        newpoint.save(function(err){
            if(err)
                res.send(err);

            // Uses Mongoose schema to run the search (empty conditions)
            var query = Fence.find({});
            query.setOptions({sort: "order"});
            query.exec(function(err, fencepoints){
            if(err)
                res.send(err);

            // If no errors are found, it responds with a JSON of all users
            res.json(fencepoints);
            });
        });
    });
    
    // Provides method for saving new users in the db
    app.post('/fencepoints/delete', function(req, res){
        console.log("Removing a new Point")
        console.log(req.body)
        
        var query = Fence;
        query.findByIdAndRemove(req.body, function(err,data){if(!err) console.log(data);});
        
        // Uses Mongoose schema to run the search (empty conditions)
        var query = Fence.find({});
        query.setOptions({sort: "order"});
        query.exec(function(err, fencepoints){
        if(err)
            res.send(err);

        // If no errors are found, it responds with a JSON of all users
        res.json(fencepoints);
        });
    });
    
//    // Retrieves JSON records for all users who meet a certain set of query conditions
//    app.post('/query/', function(req, res){
//
//        // Grab all of the query parameters from the body.
//        var lat             = req.body.latitude;
//        var long            = req.body.longitude;
//        var distance        = req.body.distance;
//        var male            = req.body.male;
//        var female          = req.body.female;
//        var other           = req.body.other;
//        var minAge          = req.body.minAge;
//        var maxAge          = req.body.maxAge;
//        var favLang         = req.body.favlang;
//        var reqVerified     = req.body.reqVerified;
//
//        // Opens a generic Mongoose Query. Depending on the post body we will...
//        var query = Fence.find({});
//
//        // ...include filter by Max Distance (converting miles to meters)
//        if(distance){
//
//            // Using MongoDB's geospatial querying features. (Note how coordinates are set [long, lat]
//            query = query.where('location').near({ center: {type: 'Point', coordinates: [long, lat]},
//
//                // Converting meters to miles. Specifying spherical geometry (for globe)
//                maxDistance: distance * 1609.34, spherical: true});
//        }
//
//        // ...include filter by Gender (all options)
//        if(male || female || other){
//            query.or([{ 'gender': male }, { 'gender': female }, {'gender': other}]);
//        }
//
//        // ...include filter by Min Age
//        if(minAge){
//            query = query.where('age').gte(minAge);
//        }
//
//        // ...include filter by Max Age
//        if(maxAge){
//            query = query.where('age').lte(maxAge);
//        }
//
//        // ...include filter by Favorite Language
//        if(favLang){
//            query = query.where('favlang').equals(favLang);
//        }
//
//        // ...include filter for HTML5 Verified Locations
//        if(reqVerified){
//            query = query.where('htmlverified').equals("Yep (Thanks for giving us real data!)");
//        }
//
//        // Execute Query and Return the Query Results
//        query.exec(function(err, users){
//            if(err)
//                res.send(err);
//
//            // If no errors, respond with a JSON of all users that meet the criteria
//            res.json(users);
//        });
//    });
};  