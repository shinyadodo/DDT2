/*global escape: true */
//var string = "blah";
//string = escape(string);

Pebble.addEventListener("showConfiguration", function(e) 
{
	Pebble.openURL("https://shinya-dodo.github.io/pebble/configurationV207.html");
});

var Base64 = 
{
	_keyStr : "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=",
	//метод для раскодировки из base64 
	decode : function (input) 
	{
		var output = "";
		var chr1, chr2, chr3;
		var enc1, enc2, enc3, enc4;
		var i = 0;
		input = input.replace(/[^A-Za-z0-9\+\/\=]/g, "");
		while (i < input.length) 
		{
			enc1 = this._keyStr.indexOf(input.charAt(i++));
			enc2 = this._keyStr.indexOf(input.charAt(i++));
			enc3 = this._keyStr.indexOf(input.charAt(i++));
			enc4 = this._keyStr.indexOf(input.charAt(i++));
			chr1 = (enc1 << 2) | (enc2 >> 4);
			chr2 = ((enc2 & 15) << 4) | (enc3 >> 2);
			chr3 = ((enc3 & 3) << 6) | enc4;
			output = output + String.fromCharCode(chr1);
			if( enc3 != 64 )
			{
				output = output + String.fromCharCode(chr2);
			}
			if( enc4 != 64 ) 
			{
				output = output + String.fromCharCode(chr3);
			}
		}
	return output;
	}
};
function b64_to_utf8( str ) 
{
	var	str1=str.replace(/ +/g, '+');
	return decodeURIComponent(escape(Base64.decode( str1 )));
}

Pebble.addEventListener('webviewclosed',
function(e) 
{
	console.log('Configuration window returned: ', e.response);
	var res = JSON.parse(b64_to_utf8(e.response)); 
	console.log('Configuration window returned: ', JSON.stringify(res));
	
	Pebble.sendAppMessage(res, 
	function(e) 
	{
		console.log("Successfully delivered message with transactionId=" + e.data.transactionId);
	},
	function(e)
	{
		console.log("Unable to deliver message with transactionId=" + e.data.transactionId  + " Error is: " + e.error.message);
	});
});
