
# by default, bro only care the software in local ip range.

# add vulnerable http client/server ftp client/server ssh client/server here 
global chrome_version_range : Software::VulnerableVersionRange = [$max = [$major=34, $minor=0, $minor2=1847, $minor3=116]];
redef Software::vulnerable_versions =
               table(
                     ["Chrome"] = set(chrome_version_range)
                    );



redef Software::interesting_version_changes += {
                 "Chrome",
                 "Apache"
               };
