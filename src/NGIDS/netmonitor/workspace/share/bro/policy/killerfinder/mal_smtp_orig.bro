@load policy/protocols/smtp/detect-suspicious-orig

## Places where it's suspicious for mail to originate from represented
## as all-capital, two character country codes (e.g., US).  It requires
## libGeoIP support built in.
#        const suspicious_origination_countries: set[string] = {} &redef;
#        const suspicious_origination_networks: set[subnet] = {} &redef;


redef SMTP::suspicious_origination_networks += {
    110.110.110.0/8
};

