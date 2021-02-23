using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Builder;
using Microsoft.AspNetCore.Hosting;
using Microsoft.AspNetCore.Http;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Microsoft.AspNetCore.SignalR;
using WinRTC_SignalR.SignalRHub;
//using Owin;
using Microsoft.Extensions.Configuration;

namespace Signaling_Hub
{
    public class Startup
    {
        // This method gets called by the runtime. Use this method to add services to the container.
        // For more information on how to configure your application, visit https://go.microsoft.com/fwlink/?LinkID=398940

        public Startup(IConfiguration configuration)
        {
            Configuration = configuration;
        }

        public IConfiguration Configuration { get; }
        public void ConfigureServices(IServiceCollection services)
        {

            services.AddCors(options =>
            {
                options.AddPolicy("AllowCors", builder =>
                {
                    builder.WithOrigins("*").AllowAnyHeader().AllowAnyMethod();
                });
            });
            services.AddControllers();

            services.AddSignalR()
                .AddAzureSignalR()
                .AddJsonProtocol(options => {
                    options.PayloadSerializerOptions.PropertyNamingPolicy = null;
                });

            //services.AddAzureSignalR(connectionString);
        }

        // This method gets called by the runtime. Use this method to configure the HTTP request pipeline.
        public void Configure(IApplicationBuilder app, IWebHostEnvironment env)
        {
            if (env.IsDevelopment())
            {
                app.UseDeveloperExceptionPage();
            }

            app.UseCors("AllowCors");
            //app.UseHttpsRedirection();
            app.UseFileServer();
            app.UseRouting();
            app.UseAuthorization();




            app.UseEndpoints(route =>
            {
                route.MapHub<AzureHub>("/azure", (config) =>
                {
                });

                route.MapGet("/azure", async context =>
                {
                    await context.Response.WriteAsync("connected");
                });
            });

            //app.UseAzureSignalR(routes =>
            //{
            //    routes.MapHub<AzureHub>("/azurehub");
            //    routes.MapHub("/azurehub", async context =>
            //    {
            //        await context.Response.WriteAsync("connected");
            //    });
            //});

        }

    }
}
