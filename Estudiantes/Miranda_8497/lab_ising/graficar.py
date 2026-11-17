import pandas as pd
import matplotlib.pyplot as plt

# --- Ising: M vs H ---
d = pd.read_csv("ising.csv")
fig, ax = plt.subplots(1, 2, figsize=(11, 4))
for T, g in d.groupby("T"):
    p = ax[0].plot(g.H, g.M_mc, "o", ms=3, label=f"MC  T={T:g} K")
    ax[0].plot(g.H, g.M_teo, "-", lw=1, color=p[0].get_color())
    ax[1].plot(g.H / T, g.M_mc, "o", ms=3, label=f"T={T:g} K")
x = d.H / d["T"]
ax[1].plot(sorted(x), [__import__("math").tanh(v) for v in sorted(x)], "k-", label="tanh(H/T)")
ax[0].set(xlabel="H (G)", ylabel=r"$\langle M\rangle/N\mu_0$", title="Magnetización vs H")
ax[1].set(xlabel="H/T", ylabel=r"$\langle M\rangle/N\mu_0$", title="Vs temperatura reducida")
for a in ax: a.legend(fontsize=8); a.grid(alpha=.3)
plt.tight_layout(); plt.savefig("ising.png", dpi=150)

# --- Boltzmann DM ---
b = pd.read_csv("boltzmann.csv")
plt.figure(figsize=(6, 4))
for lam, g in b.groupby("lambda"):
    plt.loglog(g.x, g.Y, label=fr"$\lambda$={lam:.0e}")
g0 = b[b["lambda"] == b["lambda"].iloc[0]]
plt.loglog(g0.x, g0.Yeq, "k--", label=r"$Y_{eq}$")
plt.ylim(1e-14, 1e-1); plt.xlabel("x = m/T"); plt.ylabel("Y")
plt.legend(fontsize=8); plt.grid(alpha=.3); plt.tight_layout()
plt.savefig("boltzmann.png", dpi=150)
plt.show()
