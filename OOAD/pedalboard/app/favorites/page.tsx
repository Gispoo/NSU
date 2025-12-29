import { createClient } from "@/lib/supabase/server";
import { redirect } from "next/navigation";

export const revalidate = 0; // избранное лучше без кеша

export default async function FavoritesPage() {
  const supabase = await createClient();

  const { data: authData, error: authError } = await supabase.auth.getUser();
  const user = authData?.user;

  if (authError || !user) {
    redirect("/auth/login");
  }

  // Достаём избранное и сразу подтягиваем связанный pedalboard
  const { data, error } = await supabase
    .from("favorites")
    .select(
      `
      created_at,
      pedalboards:pedalboard_id (
        id,
        title,
        short_desc,
        image_url,
        created_at
      )
    `
    )
    .eq("user_id", user.id)
    .order("created_at", { ascending: false });

  if (error) {
    return (
      <main style={{ padding: 24 }}>
        <h1>Ошибка загрузки избранного</h1>
        <pre>{JSON.stringify(error, null, 2)}</pre>
      </main>
    );
  }

  const boards = (data ?? [])
    .map((row: any) => row.pedalboards)
    .filter(Boolean);

  return (
    <main style={{ padding: 24 }}>
      <h1>Избранное</h1>

      {boards.length === 0 ? (
        <p>Пока пусто. Добавь что-нибудь ❤️</p>
      ) : (
        <div style={{ display: "grid", gap: 12 }}>
          {boards.map((b: any) => (
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
