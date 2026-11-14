#!/usr/bin/env python
# -*- coding: utf-8 -*-
#----------------------------------------------------------------------------------------------------------
#
#    SCRIPT IA google
#
#----------------------------------------------------------------------------------------------------------
# Adapter pour l'habillage des PanelWindow
#
# Il faut modifer la variable "nom"
# 
# (TODO)  choisir le repertoire des destination  
#
#----------------------------------------------------------------------------------------------------------
import os
from gimpfu import *
#----------------------------------------------------------------------------------------------------------
#
#
#----------------------------------------------------------------------------------------------------------
REP="/home/rene/programmes/opengl/lib-wmcgl/images"
#----------------------------------------------------------------------------------------------------------
#
#
#----------------------------------------------------------------------------------------------------------
def plugin_decouper_coordonnees(image, drawable, dossier_export, nom_de_fichier):
    # Vérifier si le dossier d'exportation existe, sinon le créer
    if not os.path.exists(dossier_export):
        os.makedirs(dossier_export)

    # --- CONFIGURATION DES DIRECTIVES DE DÉCOUPE ---
    # Définissez ici vos sous-images sous la forme : (X, Y, Largeur, Hauteur)
    # Vous pouvez ajouter ou modifier autant de lignes que vous le souhaitez !
    zones_de_decoupe = [
        (0,   0, 16, 16),       # Zone 1 : Coin haut gauche (Ex: petite miniature)
        (16,  0, 32, 16),     # Zone 2 : Taille moyenne à côté
        (48,  0, 16, 16),     # Zone 2 : Taille moyenne à côté

        (0,  16, 16, 32),       # Zone 1 : Coin haut gauche (Ex: petite miniature)
        (48, 16, 16, 32),     # Zone 2 : Taille moyenne à côté

        (0,  48, 16, 16),       # Zone 1 : Coin haut gauche (Ex: petite miniature)
        (16, 48, 32, 16),     # Zone 2 : Taille moyenne à côté
        (48, 48, 16, 16)     # Zone 2 : Taille moyenne à côté

    ]
    
    nom = nom_de_fichier + "_"
    noms = [
        nom+"ul.png",
        nom+"u.png",
        nom+"ur.png",

        nom+"l.png",
        nom+"r.png",

        nom+"dl.png",
        nom+"d.png",
        nom+"dr.png"
    ]

    compteur = 0

    # Parcourir chaque zone définie ci-dessus
    for (x, y, largeur, hauteur) in zones_de_decoupe:
        
        # Sécurité : vérifier que la zone ne dépasse pas les dimensions de l'image réelle
        if x >= image.width or y >= image.height:
            continue
            
        # Ajuster la largeur et hauteur si elles dépassent le bord de l'image
        largeur_ajustee = min(largeur, image.width - x)
        hauteur_ajustee = min(hauteur, image.height - y)

        # 1. Dupliquer l'image d'origine pour travailler sur une copie propre
        image_recadree = pdb.gimp_image_duplicate(image)
        
        # 2. Recadrer la copie selon la zone demandée
        pdb.gimp_image_crop(image_recadree, largeur_ajustee, hauteur_ajustee, x, y)

        # 3. Fusionner les calques visibles pour l'exportation
        calque_final = pdb.gimp_image_merge_visible_layers(image_recadree, CLIP_TO_IMAGE)

        # 4. Préparer le nom du fichier de sortie (ex: zone_1_300x200.png)
        #nom_fichier = "vert{0}_{1}x{2}.png".format(compteur, largeur_ajustee, hauteur_ajustee)
        nom_fichier = noms[compteur]
        chemin_complet = os.path.join(dossier_export, nom_fichier)

        # 5. Enregistrer le morceau en PNG
        pdb.file_png_save_defaults(image_recadree, calque_final, chemin_complet, nom_fichier)

        # 6. Libérer la mémoire en supprimant l'image temporaire
        pdb.gimp_image_delete(image_recadree)

        compteur += 1

    # Rafraîchir l'interface de GIMP et envoyer un message de confirmation
    gimp.displays_flush()
    gimp.message("Decoupe reussie ! {0} sous-images enregistrees.".format(compteur - 1))

#----------------------------------------------------------------------------------------------------------
#
#
#----------------------------------------------------------------------------------------------------------
# Enregistrement du script dans GIMP
register(
    "python_fu_decouper_coordonnees",
    "Decouper l'image en 8 parties",
    "Decoupe l'image selon une liste de rectangles de tailles variées configurée dans le code.",
    "Rene NEGRE",
    "GPL",
    "2026",
    "PanelWindow 64x64 => 16x16 32x16 ...",
    "*", 
    [
        (PF_IMAGE, "image", "Image courante", None),
        (PF_DRAWABLE, "drawable", "Calque courant", None),
        (PF_DIRNAME, "dossier_export", "Dossier d'exportation :", os.path.expanduser(REP)),
        (PF_STRING, "nom_de_fichier", "Nom de fichier : ", "...")
    ],
    [],
    plugin_decouper_coordonnees,
    menu="<Image>/Filters/MesScripts"
)
#----------------------------------------------------------------------------------------------------------
#
#
#----------------------------------------------------------------------------------------------------------
main()

