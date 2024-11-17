#ifndef ARBREAVL_ARBREAVL_H
#define ARBREAVL_ARBREAVL_H

#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include "lights.h"  // Ensure this includes the LightData struct

class Arbre
{
public:
    Arbre();

    bool estVide() const;
    const Light::LightData & max() const;
    const Light::LightData & min() const;
    long nbNoeuds() const;
    int hauteur() const;
    bool appartient(const Light::LightData &) const;
    void enfants(const Light::LightData &) const;
    void insererAVL(const Light::LightData &);
    void enleverAVL(const Light::LightData &);
    std::vector<Light::LightData> parcoursSymetrique() const;

    // Set the camera position for distance calculations
    void setCameraPosition(const glm::vec3& position); //Dont know is needed here

private:
    class Noeud
    {
    public:
        Light::LightData data;
        Noeud *gauche;
        Noeud *droite;
        int hauteur;

        explicit Noeud(const Light::LightData &d)
            : gauche(nullptr),
              data(d),
              droite(nullptr),
              hauteur(0)
        {
        }
    };

    Noeud *racine; // racine de l'arbre
    long cpt;      // Nombre de noeuds dans l'arbre
    glm::vec3 cameraPosition; // Position of the camera

    // Helper functions
    void _parcoursSymetrique(Noeud *, std::vector<Light::LightData> &) const;
    void _auxInsererAVL(Noeud *&, const Light::LightData &);
    void _auxEnleverAVL(Noeud *&, const Light::LightData &);
    void _enleverSuccMinDroite(Noeud *);
    Noeud *_auxAppartient(Noeud *arbre, const Light::LightData &) const;

    int _hauteur(Noeud *) const;
    void _balancer(Noeud *&);
    void _zigZigGauche(Noeud *&);
    void _zigZigDroit(Noeud *&);
    void _zigZagGauche(Noeud *&);
    void _zigZagDroit(Noeud *&);

    bool _debalancementAGauche(Noeud *arbre) const;
    bool _debalancementADroite(Noeud *arbre) const;
    bool _sousArbrePencheAGauche(Noeud *arbre) const;
    bool _sousArbrePencheADroite(Noeud *arbre) const;

    // Helper for calculating distance to camera
    float calculateDistance(const glm::vec3 &a, const glm::vec3 &b) const;
};

#endif // ARBREAVL_ARBREAVL_H