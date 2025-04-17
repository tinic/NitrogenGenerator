# NitrogenGenerator



## Motivation

As a home brewer CO2 is the gas of choice for most purposes, including pushing beverages to the tap. But sometimes you don't want to just serve beer but also coldbrew coffee and other non-carbonated beverages. In most cases a simple N2 cylinder does the job. But given that air is 78% nitrogen why not just extract it from the air? Recently products like NitroBev 360 have introduced this possibility on a small scale, at a price. Given the huge margins these niche products have my thought was to try to build a nitrogen generator from scratch for less.

## How does a nitrogren generator work

We will be talking about membrane based systems here only.

## Scope and scale

Typically a home brewer will not need that much nitrogen. Most commerical large scale nitrogen generation equipment is built for continious N2 blanketing and is optimized for that use case. We need to carefully consider what our goals are:

- optimize for low power use
- optimize for casual use
- idle time should not consume any power
- using readily available and servicable components
- easy maintainablity overall
- reasonanle N2 output for the application
- portability of the equipment
- making use of equipment home brewers have already 

Resulting technical metrics for my application:

- regular 120V/240V power so we can use household power or a generator.
- ~50l/h (2cfh) output. That equates to about one keg per hour.
- Max 30l/h (1cfm) of compressed air requirement so we can use a small quiet portable compressor.
- Max system pressure of about 100psi. That way we can use a Cornelious keg as a holding tank. 
