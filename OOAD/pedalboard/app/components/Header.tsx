// Header.tsx с отображением email пользователя
"use client";

import { useEffect, useState } from "react";
import { createClient } from "@/lib/supabase/client";

export function Header() {
  useEffect(() => {
    console.log("HEADER RENDER", window.location.pathname);
  }, []);

  const supabase = createClient();
  const [user, setUser] = useState<any>(null);

  useEffect(() => {
    supabase.auth.getUser().then(({ data }) => {
      setUser(data.user);
    });
  }, [supabase]);

  async function logout() {
    await supabase.auth.signOut();
    window.location.href = "/";
  }

  return (
    <header
      style={{
        padding: "12px 24px",
        borderBottom: "1px solid #ddd",
        display: "flex",
        gap: 16,
        alignItems: "center",
        backgroundColor: "#f8f9fa"
      }}
    >
      <a href="/" style={{ textDecoration: "none", color: "#333" }}>
        Home
      </a>

      <a href="/favorites" style={{ textDecoration: "none", color: "#333" }}>
        Favorites
      </a>

      <a href="/boards/new" style={{ textDecoration: "none", color: "#333" }}>
        New
      </a>

      <a href="/my" style={{ textDecoration: "none", color: "#333" }}>
        My
      </a>


      <div style={{ marginLeft: "auto", display: "flex", alignItems: "center", gap: 12 }}>
        {user ? (
          <>
            <span style={{ fontSize: "14px" }}>{user.email}</span>
            <button 
              onClick={logout}
              style={{
                padding: "6px 12px",
                background: "#f44336",
                color: "white",
                border: "none",
                borderRadius: "4px",
                cursor: "pointer",
                fontSize: "14px"
              }}
            >
              Logout
            </button>
          </>
        ) : (
          <a 
            href="/auth/login"
            style={{
              padding: "6px 12px",
              background: "#4CAF50",
              color: "white",
              textDecoration: "none",
              borderRadius: "4px",
              fontSize: "14px"
            }}
          >
            Login
          </a>
        )}
      </div>
    </header>
  );
}