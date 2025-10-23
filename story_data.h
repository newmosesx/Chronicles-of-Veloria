// File: story_data.h

#include <stddef.h>

#ifndef STORY_DATA_H
#define STORY_DATA_H

// A structure to represent a single chapter.
// It has a title and an array of paragraphs.
typedef struct {
    const char *title;
    const char **paragraphs;
    const char **perspectives;
    int paragraph_count;
} Chapter;

// --- PERSPECTIVE DEFINITIONS ---
// A default for narrator-only chapters to avoid repetition.
const char *narrator_perspective[] = { "Narrator" }; // We only need one entry

// For chapters with mixed POVs, define them fully.
const char *chapter_5_perspectives[] = {
    "Narrator", "Narrator", "Narrator", "Narrator", "Narrator", "Narrator",
    "Narrator", "Narrator", "Narrator", "Narrator", "Kaelen",   "Kaelen",
    "Kaelen",   "Kaelen",   "Kaelen",   "Kaelen",   "Kaelen",   "Narrator",
    "Narrator", NULL
};

const char *chapter_6_perspectives[] = {
    "Narrator", "Narrator", "Lyra",     "Lyra",     "Lyra",     "Lyra",
    "Lyra",     "Lyra",     "Lyra",     "Lyra",     "Lyra",     "Lyra",
    "Lyra",     "Lyra",     "Lyra",     "Lyra",     "Lyra",     "Lyra",
    "Lyra",     "Lyra",     "Narrator", NULL
};

const char *chapter_7_perspectives[] = {
    "Narrator", "Narrator", "Narrator", "Kaelen",   "Kaelen",   "Kaelen",
    "Kaelen",   "Kaelen",   "Kaelen",   "Kaelen",   "Kaelen",   NULL
};

const char *chapter_8_perspectives[] = {
    "Narrator", "Narrator", "Narrator", "Narrator", "Narrator", "Narrator",
    "Narrator", "Narrator", "Narrator", "Narrator", "Narrator", "Narrator",
    "Narrator", "Joric",    "Joric",    "Joric",    "Joric",    "Joric",
    "Joric",    "Joric",    "Joric",    "Joric",    "Joric",    "Joric",
    "Joric",    "Joric",    "Joric",    "Narrator", NULL
};

const char *game_mode1_perspective[] = {
    "House Speaker", "House Speaker", 
    "1#Council Member", "1#Council Member", "1#Council Member", "1#Council Member", "1#Council Member", "1#Council Member", 
    "2#Council Member", "2#Council Member", "2#Council Member",
    "House Speaker", "House Speaker", "House Speaker", "House Speaker", "House Speaker", "House Speaker", 
    "1#Council Member", "1#Council Member",
    NULL
};

// -----------------------------------------------------------------------------
// --- DEFINE YOUR STORY CONTENT BELOW ---
// -----------------------------------------------------------------------------

// --- CHAPTER 1 PARAGRAPHS ---
// Create an array of strings for each paragraph in Chapter 1.
// The final entry MUST be NULL so we can count them automatically.
const char *chapter_1_paragraphs[] = {
    "The land of Veloria stretched like a wound across the earth. Once, it had been seven kingdoms bound by trade, honor, and uneasy truce. Now, whispers of rebellion grew in taverns, and forests carried the smell of blood more than pine.",
    "Beneath the silver moon, a young mercenary named Kaelen Duskbane trudged down a mud-caked road. His blade was old, his boots torn, but his eyes—cold gray and too knowing for twenty summers—spoke of a man who had seen death too early.",
    "He was not a hero. Not yet. Perhaps not ever.",
    "At the crossroads outside Thalor’s Gate, a crumbling fortress town, Kaelen encountered three figures who would shape his path: Lyra Veylen, a fugitive healer; Bram Thorne, a disgraced knight; and Iriah Sable, a rogue with more daggers than patience.",
    "Their meeting was not fate, but necessity. A parchment nailed to a tree declared: \"Wanted. Adventurers for hire. The Crown pays in gold and blood.\"",
    "They laughed. They scoffed. But hunger, like war, was persuasive.",
    "As night fell, the first of many horrors revealed itself. Wolves, but not of fur and fang—these were twisted, their hides peeling, their eyes burning like coals. They came from the western woods, the cursed stretch men called The Bleeding Vale.",
    "The battle was not clean. The ground became slick with gore, the air thick with screams. When it was over, Kaelen stood drenched, trembling, and silent.",
    "In the flicker of torchlight, he noticed something in the carcasses of the beasts: fragments of black iron embedded in their flesh, glowing faintly like dying embers.",
    "Not natural. Not accidental. Someone was forging monsters.",
    "The four strangers locked eyes. Adventurers. Mercenaries. Survivors. And perhaps, unwillingly, rebels. The storm was gathering.",
    NULL // MUST end with NULL
};

// --- CHAPTER 2 PARAGRAPHS (TEMPLATE) ---
// To add chapter 2, just copy the pattern above.
const char *chapter_2_paragraphs[] = {
    "The morning after the slaughter smelled of iron and ash. The twisted wolves lay in heaps, their blackened flesh seeping smoke as if the earth itself rejected them.",
    "Kaelen crouched beside a carcass, prying one of the glowing iron shards free with his dagger. The metal pulsed faintly, warm to the touch.",
    "\"Cursed steel,” Bram muttered, crossing himself with a knight’s faded gesture. “I’ve seen it once before, in the Black Crusades. Weapons forged not to kill, but to corrupt.\"",
    "Lyra frowned, brushing soot from her hands. “If that’s true, someone is feeding this land nightmares. Steel doesn’t just walk on four legs.”",
    "From the shadows of the crumbling gate, Iriah spat. “Doesn’t matter who made them. The parchment said coin. We bring this to the Crown, we eat.”",
    "Kaelen turned the shard in his palm, gray eyes catching the faint red glow. “And if it’s the Crown that forges them?”",
    "That silence was heavier than the dawn.",
    "They entered Thalor’s Gate, the fortress-town now little more than a husk of cracked stone and hungry mouths. Beggars lined the streets. Soldiers watched with suspicion rather than protection. Every banner of the Seven Kingdoms had once flown here; now only one remained—the crimson sun of King Althar of Veloria, stitched over too many times, fraying at the edges like his reign.",
    "Inside a tavern that smelled of mildew and desperation, the four strangers found more than drink. A notice board covered in blood-stained papers told of missing children, vanished caravans, and whispers of rebellion. Pinned beneath a crude drawing of a wolf was a faded parchment bearing the royal seal, offering a handsome reward for the capture of a \"traitorous magistrate\" named Mara Voss.",
    "Lyra’s eyes lingered on one parchment longer than the others:“By decree of the Crown: The practice of unlicensed magic is punishable by fire. Informants rewarded.”",
    "She tore it down before the others could notice her trembling hands.",
    "That night, as the tavern filled with the stench of sour ale and broken laughter, Kaelen sat apart, rolling the cursed shard between his fingers. He wasn’t listening to the bard singing of old heroes. He wasn’t watching Bram drink himself to memory’s edge, or Iriah cheat farmers at dice, or Lyra smile too brightly at frightened children.",
    "He was listening to the silence between the noises.",
    "The storm wasn’t just gathering. It was already here.",
    "And in its heart, Kaelen felt a whisper clawing at his mind. Not words, not yet—just hunger.",
    NULL // MUST end with NULL
};

const char *chapter_3_paragraphs[] = {
    "Kaelen lay awake in the rented tavern room. The shard pulsed faintly on the table beside his bedroll, red light threading through the cracks in its black surface.",
    "He had faced beasts before, but never steel that breathed. And never whispers that slid into his skull like smoke.",
    "He shut his eyes, but sleep didn’t come. It never did, not since the Siege of Halewind—the day he stopped being a soldier and started being… something else.",
    "The sound of fire always returned first. Not the crackle of wood, but the shriek of flesh melting, the collapse of stone under molten sky.",
    "Halewind had burned because the Crown demanded it. Rebels inside the city refused to yield, and Kaelen’s company was ordered to torch it.",
    "He had obeyed. He had swung the torch. He had watched women clutch children as the smoke drowned their cries.",
    "That was the day his crest—the proud falcon of Veloria—felt like a shackle. That was the day he cut it from his cloak and left it in the ashes.",
    "The shard pulsed brighter, tugging him back to the present.",
    "Kaelen pressed his palms against his eyes. Not now.",
    "But the hunger pressed harder. It didn’t use words, but it promised—strength, vengeance, the power to unmake the chains of kings.For a breath, he almost wanted to answer it.",
    "\"Can’t sleep?\"",
    "Lyra’s voice came from the doorway, soft, cautious. She stepped inside, her hand brushing the frame as if making sure it was real.",
    "\"I don’t sleep,\" Kaelen muttered.",
    "Her gaze fell to the shard. “That thing… it’s wrong.”",
    "“You’re not wrong,” he said. “But wrong things have their uses.”",
    "She tilted her head, that too-bright smile flickering for a heartbeat. “That’s the kind of thinking that gets men burned.”",
    "Kaelen finally looked at her. “Maybe I deserve it.”",
    "When she left, he didn’t pick up the shard again. He just stared at it, at the way its light made shadows stretch across the wall.",
    "The corruption wasn’t in the steel. It was in him.",
    "And he knew—whether by blade, by fire, or by the hunger clawing inside—one day it would consume him.",
    "But not tonight. Tonight he had to walk the rotten streets of Thalor’s Gate and pretend he was still a man worth following.",
    NULL // MUST end with NULL
};

const char *chapter_4_paragraphs[] = {
    "The sun clawed its way over Thalor’s Gate, a weak smear of light that did little to drive off the stink of mildew and smoke. Merchants dragged empty carts through the mud, guards leaned on spears more to menace than to protect, and children watched with hollow eyes from alleys too narrow to breathe.",
    "As they passed the main gate, two guards shoved a merchant against a wall, spilling a crate of apples into the mud. “Don’t lie to us, scum,” one of them snarled, his voice loud enough for all to hear. “We know you’ve been running supplies to the Northern rebels.” The merchant stammered denials, but the guards were not listening. It was a performance of power in a town that had little left.",
    "Kaelen tightened the strap of his cloak and moved through the streets with the others. He felt every stare, every whisper following them. Outsiders were always noticed, and in this town, notice was dangerous.",
    "Lyra walked at his side, basket of herbs cradled close like a shield. She glanced at him once, her voice low enough only for him.",
    "“You didn’t sleep.”",
    "Kaelen’s jaw tightened. “You always ask questions you don’t want answers to?”",
    "“Maybe,” she said, a faint smile ghosting her lips. “But I like knowing who’s standing next to me when the blades come out.”",
    "He looked at her, searching for mockery. There wasn’t any. Just tired honesty.",
    "“Then know this,” he said at last. “I don’t break easy.”",
    "Her hand brushed the basket tighter. “Good. Because I heal what I can, but I can’t put a man’s soul back together.”",
    "For the first time in a long while, Kaelen almost smiled.",
    "They reached the tavern square where the notice board sagged under new scraps of parchment. A man stood waiting—a farmer by his clothes, but with the hollow stare of someone emptied out.",
    "“My boy,” he said, voice cracking. “Two nights past, gone. Not the first. Won’t be the last. The guard won’t lift a finger. They say the woods take them. But I know better. Something hunts.”",
    "He looked at the four strangers, desperation bleeding into every word.",
    "“Coin’s all I’ve got, but take it. Find him. Bring him back… or at least bring him peace.”",
    "Bram muttered under his breath about “lost causes.” Iriah flipped a dagger, weighing risk against reward. But Kaelen already felt the shard in his pouch grow warm, as if it hungered for the hunt.",
    "The farmer pressed his meager purse into Iriah’s hand. She didn’t hesitate; the weight of the coins vanished into her cloak before the man could even beg them again",
    "Kaelen opened his mouth—something about plans, about needing to work as one. But the words stuck.",
    "Bram had already turned, lumbering toward the gates with the weary gait of a man who had marched under too many banners.Lyra followed, soft skirts brushing the mud, her eyes fixed not on Kaelen but on the treeline where the children had vanished.And Iriah, grinning faintly, lingered only long enough to flip the purse in her palm before slipping it away and strolling after them.",
    "Kaelen realized then—no speeches, no vows. They were already moving, already listening in their own way.",
    "So he closed his mouth, adjusted the weight of the shard in his pouch, and stepped into their shadow.",
    "By the time the sun climbed high, the four of them were moving toward the black line of trees beyond Thalor’s Gate. The town’s whispers trailed after them—fear, suspicion, envy.",
    "But none of that mattered.",
    "What mattered was the first step of their first hunt, and whether they could keep each other alive long enough to finish it.",
    NULL // MUST end with NULL
};

const char *chapter_5_paragraphs[] = {
    "The road bled into forest before noon. Thalor’s Gate vanished behind them, leaving only the mutter of ravens overhead and the sound of their boots on wet leaves.",
    "The trees here grew close, bark mottled black as though fire had once licked them. Lyra kept to Kaelen’s side, herbs clutched tight in her satchel, eyes flicking to every shadow as if something waited there.",
    "The silence pressed too heavy. No insects, no wind. Just the hush of a place that seemed to be listening.",
    "“Tracks.” Bram’s voice cut through the stillness. He crouched, steel gauntlet brushing across the soil. “Small. Child’s foot. But deeper than it should be.”",
    "Kaelen stepped closer, frown deepening. The prints sank unnaturally, as though the earth itself had tried to swallow them.",
    "“They’re not alone,” Iriah muttered, pointing with her dagger. “Claw marks, dragging along the path. Like something crawled with them.”",
    "Lyra bent, her hand trembling over the print. A faint glow leaked from her palm, then she snatched it back, clutching her wrist. She glanced around as if the trees themselves might accuse her of magic.",
    "Kaelen saw the fear in her eyes, but said nothing.",
    "Hours passed. The trail led deeper, winding into a ravine where roots knotted like black veins. The air grew heavy, tasting of iron.",
    "Kaelen reached for his blade. “We’re being followed.”",
    "The first thing he heard was the crack of branches above. He drew steel before he even saw them—shapes dropping from the canopy, their limbs too long, their skin stretched tight and gray. Wolves, once. Wolves twisted into things that wore shards like bone splinters through their flesh.",
    "The first landed hard, jaws snapping. Kaelen drove his sword into its neck, twisting until black smoke hissed out instead of blood.",
    "“Behind you!” Bram’s voice thundered.",
    "Kaelen turned, shield arm rising just in time to deflect claws. The impact rattled bone. He shoved forward, blade flashing. His strike bit deep, but the creature didn’t fall—it staggered, growled, then lunged again, hungrier.",
    "They weren’t fighting to kill. They were fighting to corrupt.",
    "Bram’s shield smashed one into a tree with the crunch of bone. Iriah darted between roots, her daggers flashing silver arcs that cut tendons and eyes. Lyra stayed close to Kaelen, whispering frantic words, her hands glowing faintly whenever he faltered—warmth stealing into his muscles just enough to keep him upright.",
    "But she dared not do more.",
    "The last wolf-thing lunged straight for her. Kaelen didn’t think. He moved. Steel caught its throat before it touched her. The shard embedded in its chest pulsed once, then cracked apart, spilling embers across the ground.",
    "When it was done, silence returned—thicker now, heavier. The woods were listening.",
    "They stood together, bloodied, gasping, and not one of them spoke. But in that silence, the pact was there: no one had run.",
    NULL // MUST end with NULL
};

const char *chapter_6_paragraphs[] = {
    "The deeper they went, the worse the forest became. The air carried whispers now, voices too soft to catch but too sharp to ignore.",
    "Children’s voices.",
    "Lyra pressed her hands to her ears once, trembling. “They’re crying,” she whispered. “They’re crying for help.”",
    "Kaelen’s grip tightened on the shard in his pouch. It throbbed in answer, as if agreeing with her.",
    "The trail ended at dusk, where the ravine opened into a clearing littered with broken stones. Once a shrine, perhaps, long forgotten. At its center, a pit yawned—dark, shallow, and smelling of ash.",
    "And from the pit came the sobbing.",
    "Lyra’s breath caught as she leaned over the edge. The sobbing wasn’t far. Just down there, in the dark. “I’m going,” she said, before the others could argue.",
    "Bram frowned, lowering a rope from his pack. “You’ll not go alone.”",
    "The rope scraped as she descended, heart hammering. When her boots touched the dirt below, she raised a hand, coaxing the faintest glow from her palm. Just enough to see. The pit walls shone with iron. Not ore—shards. Dozens of them, hammered into the stone like teeth. Huddled at the far end were two children, pale and unmoving.",
    "“Found them!” Her voice echoed too loud.",
    "That’s when the ground shifted beneath her feet. The earth split, and from it crawled something stitched of shadows and iron. A man-shape, but hollow, its ribs fused with glowing shards. Lyra stumbled back, light flaring from her hand as the thing hissed, reaching for her.",
    "“Iriah!” she shouted.",
    "A rope lashed down and the rogue slid beside her, daggers ready. Her grin was thin, sharp. “Always children, always cursed. Why not coin purses for once?”",
    "The creature struck. Lyra raised both hands, light bursting in a desperate wave. It staggered, but the strain burned her veins like fire, and she nearly collapsed. Iriah darted past her, her blade slicing tendons, dancing between its blows. But for every cut, the thing moved faster, the shards in its chest glowing brighter.",
    "As it lunged to crush Iriah, Lyra screamed, and her magic poured out, uncontrolled—searing its leg.",
    "“Not a word,” Iriah panted, meeting her eyes. “You saved me. We’ll leave it at that.”",
    "Together, they dragged the children to the rope. Bram and Kaelen hauled them up, one by one, while the two women fought to keep the thing back. At last, Kaelen’s sword pierced the hollow chest, shattering the shards in a burst of red smoke. The monster fell in pieces.",
    "When the dust cleared, the children sobbed in Lyra’s arms. Alive. But thin, marked with faint lines of black on their skin—stains that looked far too much like corruption.",
    NULL // MUST end with NULL
};

const char *chapter_7_paragraphs[] = {
    "The walk back to Thalor’s Gate was quiet. Too quiet.",
    "The children clung to Lyra. She whispered to them, soft words they barely seemed to hear. Bram carried one when the boy’s legs gave out. Iriah walked ahead, pretending not to care, but her eyes never stopped scanning the treeline.",
    "Kaelen felt the shard’s heat grow. It pulsed whenever he looked at the children’s marks.",
    "It wanted them.",
    "The farmer wept when he saw his children. Fell to his knees, kissing their soot-streaked faces. He pressed another purse of coins into Iriah’s hands, sobbing thanks.",
    "She smiled, too sweet. “Your generosity saves them twice, good man.”",
    "That night in the tavern, the mood was lighter. Bram drank deeply, Lyra tended to the children who were now sleeping soundly by the hearth, and Iriah was buying a round of ale she couldn't have afforded that morning. She laughed, a sharp, bright sound in the dim room.",
    "Kaelen watched her from his corner, turning the cursed shard over in his palm. He had seen her smile that way before—the same sweet poison she gave a mark just before lifting his purse. He hadn't seen the farmer's second payment disappear, but he didn't need to. The answer was in the cheap ale she was now pouring so freely.",
    "He didn't move to confront her. An accusation was pointless and would only fracture their fragile alliance. Instead, he filed the information away. She was not just a rogue; she was a liability. Her greed made her predictable, but it also made her reckless.",
    "He closed his fist around the shard, the metal biting into his flesh. The warmth was almost comforting. The shard didn't pretend to be anything other than what it was: a thing of hunger and power. In that, it was more honest than the woman laughing across the room.",
    "This wasn't a mission. It was a matter of managing threats. And Kaelen was beginning to realize that the most immediate ones weren't the monsters in the woods, but the people walking beside him.",
    NULL // MUST end with NULL
};

const char *chapter_8_paragraphs[] = {
    "Ruined Mill - Unknown Location - The Rebels",
    "The mill had long stopped grinding. Its wheel lay half-buried in moss, its roof caved where the lord’s men had torched it a season before. The rebels had chosen it for that very ruin: easy to hide in, and harder for a patrol to spot than a purposeful camp. Lanterns burned low, the light indifferent to the shapes huddled within. Folks came and went like shadows—smiths, a handful of men who’d held soldiers’ spears once, women who learned how to make a weapon from a scythe.",
    "Mara Voss sat on a splintered beam, mapping routes in dust. Her hands were thin and callused; she had the look of someone who had once put papers to the law and found the law’s mouth full of blood. When she spoke, everyone listened.",
    "“You take a market one day, hold it two,” she told them. “You let the folks trade, you keep the mills running. You don’t become the tyrant. We hold what helps people live, not what lets us lord over them.”",
    "She introduced Joric Fen then—a man whose uniform had been cut into tatters but whose stride still had a captain’s rhythm. Joric had been a garrison captain once, which made him useful. He spoke in quiet, measured tones, always a calculation in his jaw.",
    "“Tomorrow we strike the north road,” he said. “The convoy that takes tents and grain to Aldren’s garrison. We slow that and the garrison starves sooner. We free three prisoners at an outpost on its flank. We watch for the Grey Company. If they show, we pull.”",
    "They left when the moon was lean, moving like a shadow with a purpose. Joric took point with three men close behind him, his face set like flint. They waited until the convoy slowed by the ford, lanterns bobbing like will-o’-the-wisps.",
    "The world did the thing it always did right before a fight: the air crowded tight.",
    "Joric did the thing he’d done until the Crown had cut him down: count, command, close. The river’s hiss, the creak of cart-wood, the grunt of a mule—all the ordinary noises of a supply run that would never be ordinary again. He gave the signal.",
    "He moved and so did the men behind him. They smashed the wagon’s near wheel with a timber swing, splinters crying out. The first soldier swore loud enough to startle the horses. Then blades were out: short, hard, practiced. Joric’s hand followed the cadences of dozens of skirmishes, leaving no room for the romantic. There was only space for survival.",
    "A captain turned, eyes wide at the black shapes surrounding him. He was braver than his men, carrying himself like someone who had eaten courage for breakfast. He raised a sword. Joric raised his faster.",
    "He met the captain’s blade, the shock running up his arm like a live thing. He favored the weak side of the man’s guard and struck low. The captain bled out into the dust. The two men Joric had chosen to flank him took the cart driver by surprise. They had designed this to be quick; surprise makes the slow swift.",
    "They took the convoy in under fifteen breaths, killing no more than they had to. That was another rule: keep the countryside angry at their masters, not at them. They took the grain, the tents, and the sealed chest with the officers’ pay. They unshackled the prisoners.",
    "At the far edge of the camp, one of his men came to him coughing, blood around his lips. “Map,” the man said, handing Joric the manifest. The ink was blurred by rain. The name of a place he’d never heard before—Brethford—and a list of outposts scheduled to receive the shards.",
    "The word landed in him like cold iron.",
    "There’s a moment in every fight: when the rush stops and the body count has weight. They counted what they lost. They counted what they carried. The manifest was new weight. They ghosted back under the moonlight.",
    "By dawn, the wagons were hidden. The freed prisoners staggered and ate. One of them, a lanky boy with a fever, clutched the hem of Mara’s coat. “This is only one road,” he said. “Aldren has twenty more.”",
    "Mara’s face showed no despair, only the calculus of a woman who found the world lacking mercy. “Then we get twenty more roads,” she said. “We build a map the King can’t read.”",
    "They had blood on their hands, and it was not the kind they could scrub with water. They had started something, and it smelled like fear and possibility.",
    NULL // MUST end with NULL
};

const char *game_mode1[] = {
    "\"Good day your Majesty.\"",
    "\"With his Majesty the Emperor here, we may commmence today's assambly. Speak.\"",
    "\"Good day your Majesty, I hate to be the bearer of bad news but as we may all know here, our glorious empire is in dissarray.\"",
    "\"Our food production struggles to keep up with the intake, Our granaries are emtiying as we speak and REBELS...\"",
    "\"The Empire is being robbed! Looted by a bunch of rats! We are stacking issues and troubles in a disgustingly frightening speed and dare I ask my fellow members, what has the emperor done!?\"",
    "\"Our Lord, The sole ruler of all the lands. He who controls everything and everyone...\"",
    "\"has done nothing.\"",
    "\"If only-\"",
    "\"Preposterous!\"",
    "\"How dare you? infront of the emperor! Have you lost your mind? You should be behaded right this instant!\"",
    "\"How have we allowed such a fool, to speak for so long-\"",
    "\"Order!\"",
    "\"Order!\"",
    "\"This has certainly caught many of us by surpise, including me, but allow me restate the seventh rule of the House.\"",
    "\"And so it states 'All ranks and formalities can be dropped within the house, in which, only the house speaker retains its authority within the house. Anyone in capable of staying in the house, will be allowed to leave. At the consequence of implying an agreement with the presecutor'\"",
    "\"As per the rules, nothing was broken, anyone may speak as they please aslong as I am here.\"",
    "\"You may continue your speech. But let me warn you, some opinions may backfire\"",
    "\"Thank you, Speaker. I will have it in mind.\"",
    "\"If only the the Emperor acted as we notified, our people wouldn't be in so much pain! but... it begs the question, what was your Majesty doing while the country is in crisis?\"",
    NULL // MUST end with NULL
};



// --- THE MASTER STORY ARRAY ---
// This array holds the chapters. Add new chapters here.
const Chapter story[] = {
    {
        "Chapter One: The Gathering Storm",
        chapter_1_paragraphs,
        narrator_perspective,
        (sizeof(chapter_1_paragraphs) / sizeof(chapter_1_paragraphs[0])) - 1
    },
    {
        "Chapter Two: Ashes at Dawn",
        chapter_2_paragraphs,
        narrator_perspective,
        (sizeof(chapter_2_paragraphs) / sizeof(chapter_2_paragraphs[0])) - 1
    },
    {
        "Chapter Three: Shadows Beneath the Crest",
        chapter_3_paragraphs,
        narrator_perspective,
        (sizeof(chapter_3_paragraphs) / sizeof(chapter_3_paragraphs[0])) - 1
    },
    {
        "Chapter Four: The Price of Coin",
        chapter_4_paragraphs,
        narrator_perspective,
        (sizeof(chapter_4_paragraphs) / sizeof(chapter_4_paragraphs[0])) - 1
    },
    {
        "Chapter Five: The Woods That Listen",
        chapter_5_paragraphs,
        chapter_5_perspectives,
        (sizeof(chapter_5_paragraphs) / sizeof(chapter_5_paragraphs[0])) - 1
    },
    {
        "Chapter Six: Hollow Voices",
        chapter_6_paragraphs,
        chapter_6_perspectives,
        (sizeof(chapter_6_paragraphs) / sizeof(chapter_6_paragraphs[0])) - 1
    },
    {
        "Chapter Seven: The Price of Rescue",
        chapter_7_paragraphs,
        chapter_7_perspectives,
        (sizeof(chapter_7_paragraphs) / sizeof(chapter_7_paragraphs[0])) - 1
    },
    {
        "Chapter Eight: Embers in the North",
        chapter_8_paragraphs,
        chapter_8_perspectives,
        (sizeof(chapter_8_paragraphs) / sizeof(chapter_8_paragraphs[0])) - 1
    },
    {
        "The Kingom suffers: The Conflict at Home",
        game_mode1,
        game_mode1_perspective,
        (sizeof(chapter_8_paragraphs) / sizeof(chapter_8_paragraphs[0])) - 1
    }
};

// A helper to easily get the total number of chapters.
#define TOTAL_CHAPTERS (sizeof(story) / sizeof(story[0]))


#endif // STORY_DATA_H