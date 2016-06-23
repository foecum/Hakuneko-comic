#!/bin/bash

# post javascript

COOKIES=""
AGENT="Android"
URL="http://kissanime.to"

# load kissanime
CF="$(curl -s -i -b "$COOKIES" -A "$AGENT" "$URL")"
# cookies for next requests
COOKIES="$(echo "$CF" | grep 'Set-Cookie:' | sed 's|.*: ||g;s|;.*||g');test_enable_cookie=cookie_value"
# parse form fields from document
CHK_JSCHL="$(echo "$CF" | grep 'id="challenge-form' | sed 's|.*action="||g;s|".*||g')"
JSCHL_VC="$(echo "$CF" | grep 'name="jschl_vc' | sed 's|.*value="||g;s|".*||g')"
PASS="$(echo "$CF" | grep 'name="pass' | sed 's|.*value="||g;s|".*||g')"
# find the javascript-based equation for one of the form fields
SCRIPT="$(echo "$CF" | grep 'var t,r,a,f')"
OBJ_1="$(echo "$SCRIPT" | sed 's|.*, ||g;s|={.*||g')"
OBJ_2="$(echo "$SCRIPT" | sed 's|.*{"||g;s|":.*||g')"
SCRIPT="$(echo "$SCRIPT" | sed 's|var t,r,a,f,||g;s|"||g')"
SCRIPT_1="$SCRIPT $(echo "$CF" | grep "$OBJ_1.$OBJ_2" | sed 's|a.value = |console.log(|g;s|t.length|12)|g')" # replace t.length ('kissanime.to'.length) with 12
SCRIPT_2="require('http').createServer(function (req, res) { $SCRIPT $(echo "$CF" | grep "$OBJ_1.$OBJ_2" | sed 's|a.value = |res.writeHead(200); res.end((|g;s|t.length|12)+'"''"'); }).listen()|g')" # replace t.length ('kissanime.to'.length) with 12
SCRIPT_2="$(echo "$SCRIPT_2" | sed 's| ||g')"

# evaluate the equation using slimerjs
#echo "$SCRIPT_1" > equqtion.js
#JSCHL_ANSWER="$(~/Desktop/slimerjs-0.9.5/slimerjs equqtion.js)"
#echo "$JSCHL_ANSWER"

# evaluate the equation using jsapp.us
URL="$(curl -s -d "{\"actions\":[{\"action\":\"test\",\"code\":\"$SCRIPT_2\"}]}" "http://jsapp.us/ajax" | sed 's|.*\["||g;s|"\].*||g')"
JSCHL_ANSWER="$(curl -s $URL)"
echo "$JSCHL_ANSWER"

# FIXME: URL encode of params with special characters?

# wait a bit before sending solution for the access cookie (bypass cloudflare's fast reload protection...)
sleep 5
# send form to receive cf_clearance cookie for future requests
URL="http://kissanime.to/"
COOKIES="$(curl -s -I -b "$COOKIES" -A "$AGENT" "$URL$CHK_JSCHL?jschl_vc=$JSCHL_VC&pass=$PASS&jschl_answer=$JSCHL_ANSWER" | grep 'Set-Cookie' | sed 's|.*: ||g;s|;.*||g');usingHTML5V1=true;playerTypeMobile=html5"

# show the cf_clearance cookie
echo "$COOKIES"

echo
echo "-----"
echo

# test anime list
URL="http://kissanime.to/AnimeList"
curl -s -b "$COOKIES" -A "$AGENT" "$URL" | grep 'class="bigChar"' | sed 's|.*">||g'

echo
echo "-----"
echo

# test episode list (base 64)
URL="http://kissanime.to/M/Anime/Fairy-Tail-2014"
curl -s -b "$COOKIES" -A "$AGENT" "$URL" | grep 'asp.wrap("' | sed 's|.*asp.wrap("||g;s|");.*||g' | base64 --decode | sed "s|</div>|\n|g"

echo
echo "-----"
echo

# test episode quality
URL="http://kissanime.to/Mobile/GetEpisode"
curl -s -b "$COOKIES" -A "$AGENT" -d "eID=102209" "$URL" | base64 --decode | sed "s|</a>|\n\n|g"
