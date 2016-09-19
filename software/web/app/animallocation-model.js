// Pulls Mongoose dependency for creating schemas
var mongoose    = require('mongoose');
var Schema      = mongoose.Schema;

var Animal            = require('./animal-model.js');



//var AnimalPositionSchema = new Schema({
//    location: {type: [Number], required: true}, // [Long, Lat]
//    alerts: {type: Number, required: true},
//    shocks: {type: Number, required: true},
//    sent_at: {type: Date, required: true}
//});

// Creates a User Schema. This will be the basis of how user data is stored in the db
var AnimallocationSchema = new Schema({
    animalid: {type: Schema.Types.ObjectId, ref: 'Animal', required: true},  //ObjectId??? [{ type: Schema.Types.ObjectId, ref: 'Category' }
    location: {type: [Number], required: true}, // [Long, Lat]
    alerts: {type: Number, required: true},
    shocks: {type: Number, required: true},
    sent_at: {type: Date, required: true},
});


// Indexes this schema in 2dsphere format (critical for running proximity searches)
AnimallocationSchema.index({location: '2dsphere'});

// Exports the UserSchema for use elsewhere. Sets the MongoDB collection to be used as: "scotch-users" (adds the s)
module.exports = mongoose.model('animallocation', AnimallocationSchema);

