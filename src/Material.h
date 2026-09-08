#ifndef MATERIAL_H
#define MATERIAL_H

#include "Math.h"
#include "Ray.h"
#include "HitRecord.h"

// ─────────────────────────────────────────────────────────────────────────────
// Abstract base class
// Every material must implement scatter(). If it returns false the ray is fully
// absorbed and contributes no color to the image.
// ─────────────────────────────────────────────────────────────────────────────
class Material {
public:
    virtual ~Material() = default;

    // const Ray& rayIn    — the ray that just hit the surface (read-only)
    // const HitRecord& rec — everything we know about the hit (read-only)
    // Vec3& attenuation   — OUTPUT: how much each color channel is kept (the surface color)
    // Ray& scattered      — OUTPUT: the new ray that leaves the surface
    // returns true if a new ray was produced, false if the ray is swallowed
    virtual bool scatter(const Ray& rayIn, const HitRecord& rec, Vec3& attenuation, Ray& scattered) const = 0;
};

// ─────────────────────────────────────────────────────────────────────────────
// Schlick approximation (free function)
// At steep angles glass mostly refracts. At grazing angles it mostly reflects.
// Schlick gives us a cheap probability for which one to do.
//
// r0 = ((1 - ri) / (1 + ri))^2   — reflectance at normal incidence
// result = r0 + (1 - r0) * (1 - cosine)^5
// ─────────────────────────────────────────────────────────────────────────────
inline float schlick(float cosine, float refractionIndex) {
    // Reflectance at 0° (looking straight at the surface)
    float r0 = (1.0f - refractionIndex) / (1.0f + refractionIndex);
    r0 = r0 * r0;
    // Blend toward full reflectance as the angle becomes more grazing (cosine → 0)
    return r0 + (1.0f - r0) * std::pow(1.0f - cosine, 5.0f);
}

// ─────────────────────────────────────────────────────────────────────────────
// Lambertian — matte / diffuse surface
// Scatters the incoming ray in a random direction away from the surface.
// ─────────────────────────────────────────────────────────────────────────────
class Lambertian : public Material {
public:
    Vec3 albedo; // surface color, e.g. Vec3(0.8, 0.2, 0.2) = red-ish

    // Constructor stores the surface color
    Lambertian(const Vec3& a) : albedo(a) {}

    virtual bool scatter(const Ray& rayIn, const HitRecord& rec, Vec3& attenuation, Ray& scattered) const override {
        // Pick a random direction on the unit sphere and offset it by the surface normal.
        // This biases the scatter toward the hemisphere above the surface (Lambertian distribution).
        Vec3 scatterDirection = rec.normal + Vec3::randomUnitVector();

        // If the random vector happened to exactly cancel the normal we get a zero vector.
        // A zero-length direction causes NaNs downstream, so fall back to the normal itself.
        if (scatterDirection.nearZero()) {
            scatterDirection = rec.normal;
        }

        // The new ray starts at the hit point and travels in the scattered direction
        scattered   = Ray(rec.point, scatterDirection);
        // The surface absorbs nothing — pass the full albedo through as the color multiplier
        attenuation = albedo;
        return true; // always produces a new ray
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// Metal — mirror / blurry-mirror surface
// Reflects the ray like a mirror. fuzz > 0 adds random perturbation for blur.
// ─────────────────────────────────────────────────────────────────────────────
class Metal : public Material {
public:
    Vec3  albedo; // tint of the reflection
    float fuzz;   // 0 = perfect mirror, 1 = maximally blurry

    // Clamp fuzz to [0, 1] so we never get a fuzz larger than the unit sphere
    Metal(const Vec3& a, float f) : albedo(a), fuzz(f < 1.0f ? f : 1.0f) {}

    virtual bool scatter(const Ray& rayIn, const HitRecord& rec, Vec3& attenuation, Ray& scattered) const override {
        // Normalize the incoming direction, then mirror it about the surface normal
        Vec3 reflected = rayIn.direction.normalize().reflect(rec.normal);

        // Add a random point inside the unit sphere scaled by fuzz.
        // fuzz = 0 → no perturbation → perfect mirror
        // fuzz = 1 → full sphere perturbation → blurry reflection
        Vec3 fuzzedReflected = reflected + fuzz * Vec3::randomInUnitSphere();

        // Build the outgoing ray from the hit point
        scattered   = Ray(rec.point, fuzzedReflected);
        attenuation = albedo;

        // If the fuzzed direction ended up on the wrong side of the surface, discard it.
        // dot > 0 means the scattered ray points away from the surface (correct side).
        return dot(scattered.direction, rec.normal) > 0.0f;
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// Dielectric — glass / transparent material
// Either refracts (bends) the ray through the surface or reflects it, depending
// on the angle and Schlick's probabilistic approximation.
// ─────────────────────────────────────────────────────────────────────────────
class Dielectric : public Material 
{
public:
    float refractionIndex; // 1.0 = air, 1.5 = glass, 2.4 = diamond

    Dielectric(float index) : refractionIndex(index) {}

    virtual bool scatter(const Ray& rayIn, const HitRecord& rec, Vec3& attenuation, Ray& scattered) const override {
        // Glass doesn't absorb any color — white attenuation means all light passes through
        attenuation = Vec3(1.0f, 1.0f, 1.0f);

        // Snell's law: n1 * sin(θ1) = n2 * sin(θ2)
        // ratio = n1/n2. If ray is outside hitting glass: ratio = air/glass = 1/1.5
        // If ray is inside glass hitting air (back face): ratio = glass/air = 1.5
        float ratio = rec.frontFace ? (1.0f / refractionIndex) : refractionIndex;

        // We need a unit direction to compute angles correctly
        Vec3 unit_direction = rayIn.direction.normalize();

        // cos(θ) = -dot(ray, normal). Negative because ray points toward surface, normal points away.
        // fmin clamps to 1.0 to guard against floating-point values slightly above 1
        float cos_theta = std::fmin(dot(unit_direction * -1.0f, rec.normal), 1.0f);

        // sin²(θ) + cos²(θ) = 1  →  sin(θ) = sqrt(1 - cos²(θ))
        float sin_theta = std::sqrt(1.0f - cos_theta * cos_theta);

        // Snell's law can't be satisfied when ratio * sin(θ) > 1 — the refracted angle
        // would exceed 90°. This is total internal reflection (e.g. light inside glass at
        // a steep angle can't escape — it bounces back in).
        bool cannot_refract = ratio * sin_theta > 1.0f;

        Vec3 direction;

        if (cannot_refract || schlick(cos_theta, ratio) > Vec3::randomFloat()) {
            // Total internal reflection OR Schlick says reflect this sample → mirror bounce
            direction = unit_direction.reflect(rec.normal);
        } else {
            // Snell's law is satisfied → bend the ray through the surface
            direction = unit_direction.refract(rec.normal, ratio);
        }

        // The new ray starts at the hit point and travels in the chosen direction
        scattered = Ray(rec.point, direction);
        return true; // glass always produces an outgoing ray
    }
};

#endif // MATERIAL_H
