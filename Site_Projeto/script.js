// ======== ANIMAÇÃO DE ENTRADA SUAVE ========
const sections = document.querySelectorAll("section");
const observer = new IntersectionObserver(entries => {
  entries.forEach(entry => {
    if (entry.isIntersecting) {
      entry.target.classList.add("visible");
    }
  });
}, { threshold: 0.2 });

sections.forEach(section => observer.observe(section));

// ======== BOTÃO MODO CLARO/ESCURO ========
const modoBtn = document.getElementById("modo-btn");

modoBtn.addEventListener("click", () => {
  document.body.classList.toggle("dark-mode");
  if (document.body.classList.contains("dark-mode")) {
    modoBtn.textContent = "☀️ Modo Claro";
  } else {
    modoBtn.textContent = "🌙 Modo Escuro";
  }
});
