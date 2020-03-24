const mongoose = require('mongoose');
mongoose.connect('mongodb://localhost/school', { useNewUrlParser: true ,useUnifiedTopology: true})
    .then( doc => {
        console.log('db connet success!!!');
    }).catch(err => {
        console.log('db connet fail!!!');
    });

    