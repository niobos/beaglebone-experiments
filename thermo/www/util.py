from math import log, exp


a = 17.625
b = 243.04


def gamma(T, RH):
    g = (a * T / (b + T)) + log(RH/100.0)
    return g


def rh_to_dewpoint(temperature, relative_humidity):
    """
    Converts relative humidity [0;100] to dewpoint.
    Temperatures are in degC
    """
    g = gamma(temperature, relative_humidity)
    return (b * g) / (a - g)


def dewpoint_to_rh(temperature, dewpoint):
    """
    Converts dewpoint to relative humidity [0;100]
    Temperatures are in degC
    """
    return 100 * exp(a * dewpoint / (b + dewpoint)) / exp(a * temperature / (b + temperature))
