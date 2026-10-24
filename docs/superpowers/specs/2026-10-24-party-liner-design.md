# Rhythm Ship: voxel party ocean liner

Date: 2026-10-24

## Approved direction

The user selected an adapted game liner based on the supplied party-deck reference rather than a historically exact Titanic. Preserve the reference silhouette: black hull, red lower band, ivory superstructure, warm ochre funnels with dark caps, open party deck and elevated DJ stage. Three funnels match the image; this is not a historical reconstruction.

## Deliverable and scope

Author an editable Blockbench free-format project at `gbResources/rhythm_party_liner.bbmodel`, a compact palette/pixel texture atlas, and overview/gameplay-side previews. Use a dedicated Blockbench instance so existing projects are preserved. Reuse the established local Blockbench authoring workflow. No image-generation replacement for the actual 3D asset.

Model the full liner: stepped tapered bow and rounded stepped stern, layered promenades, bridge windows, repeated warm portholes, lifeboats, three funnels, masts and festoon fixtures. At the stern, reserve a broad unobstructed dance floor with a raised stage facing it, DJ counter/decks, paired speaker stacks, framed screen, side stairs, railings, planters and lamps. Decorations must stay outside the dance space and sightline to the DJ.

Use named groups: hull, decks, superstructure, funnels, lifeboats, railings, lamps, dance_floor, stage, dj_console, speakers, screen, stairs, props. Split static colored construction from emissive-looking surfaces so later engine integration can assign materials/animations. Texture colors alone do not create runtime lighting.

## Approaches

Chosen: adapt the reference as a stylized game liner, with generous playable space and recognizable silhouette. Alternative considered: historical Titanic proportions and four funnels; the user chose the game adaptation. Dense one-cube-per-pixel construction is unnecessary: use larger cuboids/stepped sections and repeated economical details. Target approximately 15,000–35,000 triangles, report the actual count, and avoid invisible decorative detail.

Use the game's Y-up, negative-Z-toward-DJ convention. Record model dimensions, deck height, stage position and scale in the asset notes. Keep the dance floor suitable for the existing 30–50-character crowd; do not put pillars in that space.

## Boundary and verification

This stage delivers the model, editable groups/textures and visual previews. Runtime ship replacement, camera/layout changes, ocean/sky lighting changes and new effects are a separate step. Inspect the actual model in Blockbench, verify saved-file reload, textures, group membership, dimensions, finite geometry, triangle count and the dance area's clearance. Render/inspect both an overall silhouette and the stage/deck view. Do not claim in-game integration from a preview render.
