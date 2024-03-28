module.exports = require('bindings')('cryptonote-zano.node')

const bignum  = require('bignum');

module.exports.baseDiff = function() {
  return bignum('FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF', 16);
};
