// File: characters.h

#ifndef CHARACTERS_H
#define CHARACTERS_H

// A structure to represent a single character.
typedef struct {
    const char *name;
    const char *title;
    const char *description;
} Character;

// -----------------------------------------------------------------------------
// --- DEFINE YOUR CHARACTER INFO BELOW ---
// -----------------------------------------------------------------------------

const Character characters[] = {
    {
        "Kaelen Duskbane",
        "The Mercenary",
        "A young mercenary with cold gray eyes that have seen too much. His blade is old and his past is shrouded, but his skills in combat are undeniable. He is pragmatic and slow to trust, driven by survival more than honor."
    },
    {
        "Lyra Veylen",
        "The Fugitive Healer",
        "A skilled healer on the run from the very order that trained her. She possesses a rare form of magic that can mend flesh and bone, but using it risks revealing her location to her pursuers. She is compassionate but fiercely protective of her secrets."
    },
    {
        "Bram Thorne",
        "The Disgraced Knight",
        "Once a decorated knight of the King's Guard, Bram was exiled after a catastrophic failure of duty. He is a mountain of a man, clad in dented armor and burdened by guilt. He seeks redemption, but often finds only the bottom of a tavern mug."
    },
    {
        "Iriah Sable",
        "The Rogue",
        "A rogue whose sharp wit is matched only by the numerous daggers concealed on her person. Iriah grew up on the streets, learning to rely on no one but herself. She is quick, resourceful, and has little patience for authority or long-winded speeches."
    }
};

// A helper to easily get the total number of characters.
const int TOTAL_CHARACTERS = sizeof(characters) / sizeof(characters[0]);


#endif // CHARACTERS_H