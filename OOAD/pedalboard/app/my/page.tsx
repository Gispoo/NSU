import { createClient } from "@/lib/supabase/server";
import { redirect } from "next/navigation";

export const revalidate = 0;

export default async function MyBoardsPage() {
  const supabase = await createClient();

  const { data: authData } = await supabase.auth.getUser();
  const user = authData.user;

  if (!user) redirect("/auth/login");

  const { data, error } = await supabase
    .from("pedalboards")
    .select("id, title, short_desc, image_url, created_at")
    .eq("user_id", user.id)
    .order("created_at", { ascending: false });

  if (error) {
    return (
      <main style={{ padding: 24 }}>
        <h1>Ошибка</h1>
        <pre>{JSON.stringify(error, null, 2)}</pre>
      </main>
    );
  }

  return (
    <main style={{ padding: 24 }}>
      <h1>Мои педалборды</h1>

      {(data?.length ?? 0) === 0 ? (
        <p>Пока пусто. Создай первый педалборд.</p>
      ) : (
        <div style={{ display: "grid", gap: 12 }}>
          {data!.map((b) => (
            <a
              key={b.id}
              href={`/boards/${b.id}`}
              style={{
                display: "flex",
                gap: 12,
                border: "1px solid #ddd",
                padding: 12,
                borderRadius: 10,
                textDecoration: "none",
                color: "inherit",
              }}
            >
              {b.image_url ? (
                <img
                  src={b.image_url}
                  alt={b.title}
                  style={{
                    width: 96,
                    height: 96,
                    objectFit: "cover",
                    borderRadius: 8,
                  }}
                />
              ) : (
                <div
                  style={{
                    width: 96,
                    height: 96,
                    borderRadius: 8,
                    background: "#eee",
                  }}
                />
              )}

              <div>
                <div style={{ fontWeight: 700 }}>{b.title}</div>
                <div style={{ opacity: 0.8 }}>{b.short_desc}</div>
              </div>
            </a>
          ))}
        </div>
      )}
    </main>
  );
}
