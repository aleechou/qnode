const crypto = require('crypto');

qnode.utils = {
    md5: function(value) {
        return crypto.createHash('md5').update(value).digest('hex');
    }
}
