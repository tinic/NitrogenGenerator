# NitrogenGenerator



## Motivation

As a home brewer CO2 is the gas of choice for most purposes, including pushing beverages to the tap. But sometimes you don't want to just serve beer but also coldbrew coffee and other non-carbonated beverages. In most cases a simple N2 cylinder does the job. But given that air is 78% nitrogen why not just extract it from the air? Recently products like NitroBev 360 have introduced this possibility on a small scale, at a price. Given the huge margins these niche products have my thought was to try to build a nitrogen generator from scratch for less.

## How does a nitrogren generator work

We will be talking about membrane based systems here only. A membrane-based nitrogen generator works by passing compressed air through a semi-permeable membrane made of bundles of hollow polymer fibers. Oxygen and water vapor molecules, being smaller and having a higher permeation rate, pass through the membrane walls and are vented out. Nitrogen molecules, being larger and having a lower permeation rate, are retained within the fibers and collected as a high-purity nitrogen gas stream at the outlet. Purity levels can be adjusted down to about 0.5% oxygen which is appropriate for beverage service.

A membrane nitrogen generator requires a minimum amount of continuous compressed air to function effectively because the separation process relies on a consistent flow of air across the semi-permeable membranes. This continuous flow maintains the pressure differential needed for the selective permeation of oxygen and water vapor, allowing the nitrogen to be concentrated and collected. If the air supply is insufficient or intermittent, the pressure will drop, reducing the efficiency of the separation process and potentially leading to lower nitrogen purity and flow rates. Furthermore, the design of the membranes and the entire system is optimized for a specific range of air flow rates; operating below this minimum can disrupt the intended gas dynamics and performance of the generator.

A critical component of a nitrogen generator is a needle valve which is placed post nitrogen membrane. This needle valve controls the resulting nitrogen purity. Too high air flow at the membrane output will result in high oxygen levels or no oxygen filtering at all. Too low and most of the compressed air input is lost in the permeate and the effiency of the system drops off a cliff. This adjustment is highly dependent on the type and size of membrane used and drive the required continious compressed air requirement.

A nitrogen generator requires filtered air to protect its delicate internal components, primarily the semi-permeable membranes. Compressed air invariably contains contaminants like dust particles, oil aerosols, water vapor, and hydrocarbons. These contaminants can foul or block the tiny pores within the membranes of membrane-based generators, reducing their efficiency and drastically reduce lifespan. Typically any contaminants down to 0.01μm need to be filtered out of the air before it enters the membrane.

## Scope and scale

Typically a home brewer will not need that much nitrogen. Most commerical large scale nitrogen generation equipment is built for continious N2 blanketing and is optimized for that use case. We need to carefully consider what our goals are:

- optimize for low power use
- optimize for casual use
- idle time should not consume any power
- using readily available and servicable components
- easy maintainablity overall
- reasonanble N2 output for the application
- portability of the equipment
- making use of equipment home brewers have already 

Resulting technical metrics for my application:

- regular 120V/240V power so we can use household power or a generator.
- ~50l/h (2cfh) output. That equates to about one keg per hour.
- Max 30l/h (1cfm) of compressed air requirement so we can use a small quiet portable compressor.
- Max system pressure of about 100psi. That way we can use a Cornelious keg as a holding tank. 

# Selecting the components

- The core part of this build is the nitrogen membrane. For a 50l/h (2cfh) nitrogren flow the nitrogren membrane will be typically in a form a cylinder in the range of 300-500mm (12-15in) length and 40-60mm (1.5-2.5in) in diameter. 

Consult the spec pages which usually contain tables explaining the required air input vs resulting nitrogen output at different concentrations. For practical purposes I would simply ignore any nitrogen concentration under 99.5%. Typical air input vs concentrated nitrogen air output ratios are 5:1. Focus on models which are in the 50l/h range at 100psi and 99.5%+ nitrogen concentation.

Nitrogen membranes have a maximum input pressure design limit. This can range from 80psi to 150psi or higher for special applications. Above that and the membrane might disintegrate internally. Get a variant rated around 120psi or higher as we targeting 100psi as the system pressure. 

Membranes have a 'knocking' limit, i.e. maximum pressure differential going into the system when suddendly opening a valve. This is usually 20-50psi. A too high pressure differential can shatter the membrane, completely destroying it. We will control this through a needle valve on the input side. Most commerical equiment will omit this valve since those systems are usually in a well controlled environment. I assume the user (me) will make a stupid mistake at some point and I'd rather protect the expensive membrane.

Given local prices for nitrogren membranes, which can be in the many thousands of dollars, I sourced my membrane from Alibaba.com. Specifically the MNH-1512A model from KL-AIR. At the time of this writing it was $400, not including shipping and tariffs. 

Be careful when looking on eBay for used membranes. There will be a good chance that those membranes are clogged/used-up. Pretty much no seller eBay will be able to test if a membrane still works properly. Don't make the same mistake I made where I got a nifty deal on a membrane only to realize that it needs 170l/m (6cfm) to drive it. That's a LOT of air for a home gamer!

- We need a 3-stage air filter to treat the air. The stages are usually a 10μm particulate filter, a 0.1μm oil filter and a 0.01μm coalescing filter. There are many variations on that but the key is that we want a 0.01μm stage in there somewhere. If yor air contains a lot of hydrocarbons (like in an active workshop) you'd also want a activated charcoal filter in addition to the 3-stage filter. Prices range from $200 to $2000 (if you get one on McMaster). I chose a PneumaticPlus SAFU460-N04-MEP which seemed reasonbly priced.

- Most commerically available air compressors can easily provide the air required. I chose a Makita MAC100Q which is plenty sized for the membrane I chose. It's small and quiet.

- 